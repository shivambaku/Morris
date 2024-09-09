#ifndef MORRIS_ALGORITHMS_RANDOM_PLAY_HPP_
#define MORRIS_ALGORITHMS_RANDOM_PLAY_HPP_

namespace algorithm {

template<class GameType, class StateType>
class RandomPlay {
public:
    StateType Compute(StateType const & state) {
        std::mt19937_64 random_engine;
        return GameType::SimulationPolicy(state, random_engine);
    }
private:
};

} // namespace algorithm

#endif /* MORRIS_ALGORITHMS_RANDOM_PLAY_HPP_ */


