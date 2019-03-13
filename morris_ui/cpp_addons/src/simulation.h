#pragma once

#include "napi_helper.h"
#include "morris/pch.hpp"
#include "morris/games/simulation.hpp"
#include "morris/games/nine_men_morris.hpp"
#include "morris/algorithms/random_play.hpp"
#include "morris/algorithms/mcts.hpp"

class Simulation
{
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Move(napi_env env, napi_callback_info info);
    static napi_value MoveHuman(napi_env env, napi_callback_info info);
    static napi_value State(napi_env env, napi_callback_info info);
    static napi_value ListMoves(napi_env env, napi_callback_info info);
    static napi_value PlacementMoves(napi_env env, napi_callback_info info);

private:
    explicit Simulation();
    ~Simulation();

private:
    static napi_ref constructor;
    napi_env m_env;
    napi_ref m_wrapper;

    boardgame::Simulation<
                        boardgame::NineMenMorris,
                        boardgame::NineMenMorrisState,
                        boardgame::NineMenMorrisMove,
                        algorithm::MonteCarloTreeSearch<boardgame::NineMenMorris, boardgame::NineMenMorrisState>,
                        algorithm::MonteCarloTreeSearch<boardgame::NineMenMorris, boardgame::NineMenMorrisState> > * simulation_;

    friend class napi_helper;
};
