#include "MyStrategy.hpp"


model::Action MyStrategy::getAction(const model::Game& game) {
	if (Graph::getInstance() == nullptr)
		Graph::init(game);
	if (game.currentTick >= game.maxTickCount)
		Graph::cleanup();

	game_wrapper.update(game);

	if (!game.currentTick)
		return Action();

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
