#include "MyStrategy.hpp"

model::Action MyStrategy::getAction(const model::Game& game) {
	if (game.currentTick == 0)
		return Action();

	if (Graph::getInstance() == nullptr)
		Graph::init(game);

	game_wrapper.update(game);

	core.process(game_wrapper);

	Action result;

	for (Task* task : core.getAllTasks()) {
		if (task->getActorSpecialty() == game_wrapper.getMySpecialty()) {
			// std::cout << "Doing my task!" << std::endl;
			result += task->toAction();
		}
	}
	result.chooseSpecialty = game_wrapper.getNextFreeSpecialty();
	return result;
}

MyStrategy::~MyStrategy() {
	Graph::cleanup();
}
