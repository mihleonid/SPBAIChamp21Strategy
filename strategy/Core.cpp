#include "Core.h"


void Core::process(GameWrapper& game_wrapper) {
	for (int priority = 0; priority < logic_priority.size(); ++priority) {
		std::vector<Task*> to_remove;
		for (Task* task : current_tasks[priority]) {
			task->reserve(game_wrapper);
			if (task->hasStopped(game_wrapper))
				to_remove.push_back(task);
		}
		for (Task* task : to_remove) {
			current_tasks[priority].erase(task);
			delete task;
		}

		logic_priority[priority](*this, priority, game_wrapper);
	}
}

bool Core::addTask(Task *task, int priority, GameWrapper& game_wrapper) {
	bool will_execute = task->reserve(game_wrapper);
	if (will_execute) {
		current_tasks[priority].insert(task);
	}
	return will_execute;
}
