#include "MyStrategy.hpp"
// #include <exception>
#include "strategy/GameWrapper.h"


MyStrategy::MyStrategy() {}

model::Action MyStrategy::getAction(const model::Game& game)
{
	gameWrapper.update(game);
	// core.process(gameWrapper);

	Action result;

	return result;
	// return model::Action(gw.get_moves(), std::vector<model::BuildingAction>(), std::optional<model::Specialty>());
}
