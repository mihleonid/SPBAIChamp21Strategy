#include "MyStrategy.hpp"
#include "strategy/graph.h"

model::Action MyStrategy::getAction(const model::Game& game) {
	if (Graph::getInstance() == nullptr)
		Graph::init(game);
	if (game.currentTick >= game.maxTickCount)
		Graph::cleanup();

	game_wrapper.update(game);
	core.process(game_wrapper);

	Action result;

	for (Task* task : core.getAllTasks()) {
		if (task->getActorSpecialty() == game_wrapper.getMySpecialty()) {
			result += task->toAction();
		}
	}
	return result;
}
