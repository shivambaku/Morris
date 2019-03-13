#include "simulation.h"

napi_ref Simulation::constructor;

Simulation::Simulation() :
    m_env(nullptr),
    m_wrapper(nullptr)
{
    auto initial_state = []() { return boardgame::NineMenMorrisState(boardgame::Player::kRightPlayer); };

    //auto l_algorithm = algorithm::RandomPlay<boardgame::NineMenMorris, boardgame::NineMenMorrisState>();
    auto algorithm = algorithm::MonteCarloTreeSearch<boardgame::NineMenMorris, boardgame::NineMenMorrisState>(150000, 7000);

    simulation_ = new boardgame::Simulation<
                            boardgame::NineMenMorris,
                            boardgame::NineMenMorrisState,
                            boardgame::NineMenMorrisMove,
                            algorithm::MonteCarloTreeSearch<boardgame::NineMenMorris, boardgame::NineMenMorrisState>,
                            algorithm::MonteCarloTreeSearch<boardgame::NineMenMorris, boardgame::NineMenMorrisState> >
                            (initial_state(), algorithm, algorithm);
}

Simulation::~Simulation() {
    delete simulation_;

    napi_delete_reference(m_env, m_wrapper);
}

napi_value Simulation::Init(napi_env env, napi_value exports)
{
    std::vector<napi_property_descriptor> properties = {
        NAPI_METHOD_DESCRIPTOR(Move),
        NAPI_METHOD_DESCRIPTOR(MoveHuman),
        NAPI_METHOD_DESCRIPTOR(State),
        NAPI_METHOD_DESCRIPTOR(ListMoves),
        NAPI_METHOD_DESCRIPTOR(PlacementMoves)
    };

    return napi_helper::Init(env, exports, Simulation::constructor, Simulation::New, "Simulation", properties);
}

napi_value Simulation::New(napi_env env, napi_callback_info info)
{
    auto func = [] (auto & fi)
    {
        return new Simulation();
    };

    return napi_helper::New<Simulation>(env, info, Simulation::constructor, func, 0);
}

napi_value Simulation::Move(napi_env env, napi_callback_info info)
{
    napi_helper_finfo<Simulation> finfo(env, info);

    auto result = finfo.This()->simulation_->Move();

    return finfo.Return(std::get<0>(result));
}

napi_value Simulation::PlacementMoves(napi_env env, napi_callback_info info)
{
    napi_helper_finfo<Simulation> finfo(env, info);

    auto state = finfo.This()->simulation_->State();
    auto array = boardgame::NineMenMorris::PlacementMoves(state);
    auto j_array = finfo.CreateArray(array.size());
    unsigned i = 0;
    for (auto & move : array)
    {
        auto j_move = finfo.CreateObject();
        finfo.SetProperty(j_move, "source", finfo.Return(move.source));
        finfo.SetProperty(j_move, "destination", finfo.Return(move.destination));
        finfo.SetProperty(j_move, "deletion", finfo.Return(move.deletion));
        finfo.SetElement(j_array, i++, j_move);
    }

    return j_array;
}

napi_value Simulation::ListMoves(napi_env env, napi_callback_info info)
{
    napi_helper_finfo<Simulation> finfo(env, info);

    auto state = finfo.This()->simulation_->State();
    auto array = boardgame::NineMenMorris::ListMoves(state);
    auto j_array = finfo.CreateArray(array.size());
    unsigned i = 0;
    for (auto & move : array)
    {
        auto j_move = finfo.CreateObject();
        finfo.SetProperty(j_move, "source", finfo.Return(move.source));
        finfo.SetProperty(j_move, "destination", finfo.Return(move.destination));
        finfo.SetProperty(j_move, "deletion", finfo.Return(move.deletion));
        finfo.SetElement(j_array, i++, j_move);
    }

    return j_array;
}

napi_value Simulation::MoveHuman(napi_env env, napi_callback_info info)
{
    napi_helper_finfo<Simulation> finfo(env, info, 1);

    auto move_js = finfo.GetValue(0);
    unsigned source = finfo.GetUInt(finfo.GetProperty(move_js, "source"));
    unsigned destination = finfo.GetUInt(finfo.GetProperty(move_js, "destination"));
    unsigned deletion = finfo.GetUInt(finfo.GetProperty(move_js, "deletion"));

    auto move = boardgame::NineMenMorrisMove(source, destination, deletion);

    auto result = finfo.This()->simulation_->Move(move);

    return finfo.Return(std::get<0>(result));
}

napi_value Simulation::State(napi_env env, napi_callback_info info)
{
    napi_helper_finfo<Simulation> finfo(env, info);

    auto state = finfo.This()->simulation_->State();

    auto j_object = finfo.CreateObject();

    auto const & array = state.board;
    auto j_array = finfo.CreateArray(array.size());
    unsigned i = 0;
    for (auto & player: array) {
        finfo.SetElement(j_array, i++, player == boardgame::Player::kNone ?
            finfo.Return("empty") :
             player == boardgame::Player::kLeftPlayer ?
            finfo.Return("left_player") : finfo.Return("right_player"));
    }
    finfo.SetProperty(j_object, "board", j_array);

    if (state.player == boardgame::Player::kLeftPlayer) {
        finfo.SetProperty(j_object, "player", finfo.Return("left_player"));
    } else {
        finfo.SetProperty(j_object, "player", finfo.Return("right_player"));
    }

    auto left_player_remaining = state.RemainingToPlay(boardgame::Player::kLeftPlayer);
    finfo.SetProperty(j_object, "left_player_remaining", finfo.Return(left_player_remaining));

    auto right_player_remaining = state.RemainingToPlay(boardgame::Player::kRightPlayer);
    finfo.SetProperty(j_object, "right_player_remaining", finfo.Return(right_player_remaining));

    auto stage = state.Stage(state.player);
    finfo.SetProperty(j_object, "stage", finfo.Return(static_cast<int>(stage)));

    return j_object;
}