#pragma once

#include "model/Game.hpp"
#include "model/Action.hpp"
#include "strategy/GameWrapper.h"
#include "strategy/Core.h"

class MyStrategy {
public:
    model::Action getAction(const model::Game& game);
private:
	Core core;
	GameWrapper game_wrapper;
};
