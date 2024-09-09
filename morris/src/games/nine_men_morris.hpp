#ifndef MORRIS_GAMES_NINE_MEN_MORRIS_HPP_
#define MORRIS_GAMES_NINE_MEN_MORRIS_HPP_

#include "simulation.hpp"

namespace boardgame {

struct NineMenMorrisMove {
    NineMenMorrisMove(int source, int destination, int deletion) :
        source(source), destination(destination), deletion(deletion) {}

    int source = -1;
    int destination = -1;
    int deletion = -1;
};

struct NineMenMorrisState {
    static const unsigned kBoardSize = 24;

    enum class Phase {
        kPlacement,
        kMovement,
        kFreeMovement
    };

    NineMenMorrisState(Player player) : player(player) {
        std::fill(board.begin(), board.end(), Player::kNone);
    }

    unsigned RemainingToPlay(Player player) const {
        return remaining_to_play_[static_cast<unsigned>(player)];
    }

    void SetRemainingToPlay(Player player, unsigned remaining_to_play) {
        remaining_to_play_[static_cast<unsigned>(player)] = remaining_to_play;
    }

    unsigned Remaining(Player player) const {
        return remaining_[static_cast<unsigned>(player)];
    }

    void SetRemaining(Player player, unsigned remaining) {
        remaining_[static_cast<unsigned>(player)] = remaining;
    }

    Phase Stage(Player player) const {
        return phase_[static_cast<unsigned>(player)];
    }

    void SetStage(Player player, Phase phase) {
        phase_[static_cast<unsigned>(player)] = phase;
    }

    void Print() const {
        for (unsigned i = 0; i < board.size(); ++i) {
            std::cout << static_cast<int>(board[i]) << ", ";
        }
        //std::cout << '\n';
        std::cout << "Phase: " << static_cast<int>(phase_[0]) << ", " << static_cast<int>(phase_[1]) << '\n';
    }

    Player player = Player::kLeftPlayer;
    std::array<Player, kBoardSize> board;
private:
    std::array<unsigned, 2> remaining_to_play_ = { 9, 9 };
    std::array<unsigned, 2> remaining_ = { 9, 9 };
    std::array<Phase, 2> phase_ = { Phase::kPlacement, Phase::kPlacement };
};

class NineMenMorris {
public:
    const unsigned kMaxMillsSizePerPlayer = 4;

    static std::tuple<bool, Player> Winner(NineMenMorrisState const & state);
    static std::tuple<bool, std::array<double, 2>> StateValue(NineMenMorrisState const & state, unsigned depth = 0);
    static NineMenMorrisState SimulationPolicy(NineMenMorrisState const & state, std::mt19937_64 & random_engine);

    static NineMenMorrisState::Phase GetStage(NineMenMorrisState const & state, Player player);
    static std::vector<NineMenMorrisMove> ListMoves(NineMenMorrisState const & state);
    static NineMenMorrisState ApplyMove(NineMenMorrisState const & state, NineMenMorrisMove const & move);
    static std::vector<NineMenMorrisMove> PlacementMoves(NineMenMorrisState const & state);
    static std::vector<NineMenMorrisMove> MovementMoves(NineMenMorrisState const & state);
    static std::vector<NineMenMorrisMove> FreeMovementMoves(NineMenMorrisState const & state);
    static void DeletionMoves(NineMenMorrisState const & state, int source, int destination, std::vector<NineMenMorrisMove> & moves);
    static bool PartOfAMill(NineMenMorrisState const & state, unsigned destination, Player player);
private:
    // indices to all possible mill combinations
    static std::vector<std::array<unsigned, 2>> kMills;

    // all the possible moves from one spot to another
    static std::vector<std::vector<unsigned>> kNeighbors;
};

} // namespace boardgame

#endif /* MORRIS_GAMES_NINE_MEN_MORRIS_HPP_ */
