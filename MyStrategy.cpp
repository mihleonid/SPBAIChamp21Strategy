#include "MyStrategy.hpp"
#include <exception>
#include "strategy/GameWrapper.h"

GameWrapper gw;
MyStrategy::MyStrategy() {}

model::Action MyStrategy::getAction(const model::Game& game)
{
	gw.update(game);
	return model::Action(gw.get_moves(), std::vector<model::BuildingAction>(), std::optional<model::Specialty>());
}
