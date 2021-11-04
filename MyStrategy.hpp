#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "model/Game.hpp"
#include "model/Action.hpp"
#include "strategy/GameWrapper.h"

class MyStrategy {
public:
    MyStrategy();
    model::Action getAction(const model::Game& game);
private:
	GameWrapper gameWrapper;
	// Core core;
};

#endif