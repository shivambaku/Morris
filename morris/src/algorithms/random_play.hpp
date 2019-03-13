#ifndef RandomPlay_hpp
#define RandomPlay_hpp

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

#endif /* RandomPlay_hpp */


