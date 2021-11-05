#include "MyStrategy.hpp"
#include "strategy/graph.h"

model::Action MyStrategy::getAction(const model::Game& game) {
	if (Graph::getInstance() == nullptr)
		Graph::init(game);
	if (game.currentTick >= game.maxTickCount)
		Graph::cleanup();

	game_wrapper.update(game);

	// if (game.currentTick <= 3) // FIXME
	// 	return Action(); // TODO: return my specialty

	core.process(game_wrapper);

	Action result;

	for (Task* task : core.getAllTasks()) {
		if (task->getActorSpecialty() == game_wrapper.getMySpecialty()) {
			// std::cout << "Doing my task!" << std::endl;
			result += task->toAction();
		}
	}
	result.chooseSpecialty = Specialty::LOGISTICS;
	return result;
}
