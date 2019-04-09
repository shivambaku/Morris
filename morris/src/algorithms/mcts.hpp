#ifndef MORRIS_ALGORITHMS_MCTS_HPP_
#define MORRIS_ALGORITHMS_MCTS_HPP_

namespace algorithm {

template<class StateType>
struct MonteCarloNode {
    MonteCarloNode(StateType state)
    : state(state), q(0), visits(0), parent(nullptr) {
    }

    ~MonteCarloNode() {
        for (auto * child : children) {
            delete child;
        }
    }

    void AddChild(StateType const & state) {
        auto * node = new MonteCarloNode<StateType>(state);
        node->parent = this;
        children.push_back(node);
    }

    void UpdateStatistics(double value) {
        visits += 1;

        // cumulative moving average (average of all value's so far)
        q += (value - q) / visits;
    }

    bool HasChildren() {
        return !children.empty();
    }

	double Exploration(double c) {
		return visits == 0 ? std::numeric_limits<double>::max() : c * std::sqrt(2.0 * std::log(parent->visits) / visits);
	}

	void Print() {
		std::cout << "MCTS NODE: \n";
		std::cout << "Visits: " << visits << '\n';
		std::cout << "Value: " << q << '\n';
		state.Print();
	}

    StateType state;
    double q;
    unsigned visits;
    MonteCarloNode * parent;
    std::vector<MonteCarloNode*> children;
};

template<class GameType, class StateType, unsigned PlayerCount>
class MonteCarloTreeSearch {
public:
    MonteCarloTreeSearch(unsigned max_iterations = 100, long long max_time_in_milliseconds = std::numeric_limits<long long>::max(), double c = 1.0, unsigned thread_count = 0)
    : max_iterations_(max_iterations), max_time_(max_time_in_milliseconds), c_(c) {
		if (thread_count == 0) {
			unsigned concurrent_threads = std::thread::hardware_concurrency();
			thread_count_ = concurrent_threads == 0 ? 4 : concurrent_threads;
		}
		else {
			thread_count_ = thread_count;
		}
    }

	StateType Compute(StateType const & state) {

		// run multiple threads of mcts
		std::vector<std::future<MonteCarloNode<StateType>*>> futures;
		futures.reserve(thread_count_);
		for (unsigned i = 0; i < thread_count_; ++i) {
            auto seed = std::random_device{}();
			futures.push_back(std::async(std::launch::async, [i, &state, seed, this]() -> MonteCarloNode<StateType>* {
				return Compute(state, seed);
			}));
		}

		// wait for all threads to finish and get the root
		std::vector<MonteCarloNode<StateType>*> roots;
		roots.reserve(thread_count_);
		for (unsigned i = 0; i < thread_count_; ++i) {
			roots.push_back(futures[i].get());
		}

        std::mt19937 random_engine(std::random_device{}());

		// add all the visits of the children from each root
		// pick the child that maximizes the visits
		MonteCarloNode<StateType> * best_child = nullptr;
		unsigned max_visits = 0;
		size_t children_count = roots[0]->children.size();
		for (unsigned child_index = 0; child_index < children_count; ++child_index) {
			unsigned child_visits = 0;
			for (unsigned root_index = 0; root_index < roots.size(); ++root_index) {
				child_visits += roots[root_index]->children[child_index]->visits;
			}

            // change the best child by a coin flip
            if (child_visits == max_visits) {
                std::uniform_int_distribution<std::mt19937::result_type> coin_flip(0, 1);
                if (coin_flip(random_engine) == 1) {
                    best_child = roots[0]->children[child_index];
                }
            } else if (child_visits > max_visits) {
				max_visits = child_visits;
				best_child = roots[0]->children[child_index];
			}

			//roots[0]->children[child_index]->Print();
		}

		//std::cout << roots[0]->q << '\n';
		//for (unsigned i = 0; i < roots[0]->children.size(); ++i) {
		//	std::cout << roots[0]->children[i]->q << '\n';
		//}
		//std::cin.get();

		StateType best_state = best_child->state;

		// clean up the memory
		for (unsigned i = 0; i < roots.size(); ++i) {
			delete roots[i];
		}

		return best_state;
	}

	MonteCarloNode<StateType> * Compute(StateType const & state, std::mt19937_64::result_type seed) {

		std::mt19937_64 random_engine(seed);

		auto * root = new MonteCarloNode<StateType>(state);

		// expand once so the selection does not select the root
		Expand(root);

		auto start = std::chrono::high_resolution_clock::now();
		long long duration = 0;
		for (unsigned i = 0; (i < max_iterations_) && (duration < max_time_); ++i) {

			MonteCarloNode<StateType> * leaf = Select(root);
			Expand(leaf);
			std::array<double, PlayerCount> values = Simulate(leaf, random_engine);
			Backup(leaf, values);

			auto end = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		}

		return root;
	}
private:
    // select the node that has never been visited
    // if all nodes have been visited then select using exploitation / exploration
    MonteCarloNode<StateType> * Select(MonteCarloNode<StateType> * root) const {
        MonteCarloNode<StateType> * node = root;
        while (node->HasChildren()) {
            node = *std::max_element(node->children.begin(), node->children.end(), [this] (MonteCarloNode<StateType> * n1, MonteCarloNode<StateType> * n2) {
				return n1->q + n1->Exploration(c_) < n2->q + n2->Exploration(c_);
            });
        }
        return node;
    }

    // get all the next possible states and make a node for each one
    // assign those nodes as children
    void Expand(MonteCarloNode<StateType> *node) const {

		// check if game is over already
        std::tuple<bool, std::array<double, PlayerCount>> result = GameType::StateValue(node->state);
        if (std::get<0>(result) == false) return;

		auto moves = GameType::ListMoves(node->state);
		for (auto & move : moves) {
			node->AddChild(GameType::ApplyMove(node->state, move));
		}
    }

    // play a policy until we reach the final state of the game
    // return the value of the final state
	std::array<double, PlayerCount> Simulate(MonteCarloNode<StateType> *leaf, std::mt19937_64 &random_engine) {

		StateType final_state = leaf->state;
        std::tuple<bool, std::array<double, PlayerCount>> result = GameType::StateValue(final_state);

        // while the game is on going
        while (std::get<0>(result)) {
			final_state = GameType::SimulationPolicy(final_state, random_engine);
            result = GameType::StateValue(final_state);
        }

        // the value of final state
        return std::get<1>(result);
    }

    // set the value of the node that was simulated and all its parents
    void Backup(MonteCarloNode<StateType> * node, std::array<double, PlayerCount> const & values) const {

		// update all node's statistic based on their parents player
        while (node->parent != nullptr) {
            node->UpdateStatistics(values[static_cast<unsigned>(node->parent->state.player)]);
            node = node->parent;
        }

		// update root's visit count
		node->UpdateStatistics(values[static_cast<unsigned>(node->state.player)]);
    }

private:
    unsigned max_iterations_;
	long long max_time_;
    double c_;
	unsigned thread_count_;
};

} // namespace algorithm

#endif /* MORRIS_ALGORITHMS_MCTS_HPP_ */


