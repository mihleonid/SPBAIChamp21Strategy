#pragma once
#include <iostream>
#include <map>
#include <unordered_set>
#include <functional>
#include "GameWrapper.h"
#include "tasks/Task.h"
#include "tasks/DestroyTask.h"

class Core {
public:
	void process(GameWrapper&);

	bool addTask(Task*, int priority, GameWrapper&);

	std::vector<Task*> getAllTasks() {
		std::vector<Task*> result;
		for (auto& [priority, task_set] : current_tasks) {
			for (Task* task : task_set) {
				result.push_back(task);
			}
		}
		return result;
	}

private:
	// приоритет -> список заданий с таким приоритетом
	std::map<int, std::unordered_set<Task*>> current_tasks;

	// Упорядоченный список функций-элементов логики
	// Функции ((int) -> void) выполняются в порядке, заданном в этом массиве,
	// и они добавляют Task переданного приоритета
	const std::vector<std::function<void(Core&, int, GameWrapper&)>> logic_priority = {
		&Core::destroyLogic
	};

	inline void destroyLogic(int priority, GameWrapper& game_wrapper) {
		std::cerr << "DESTROY!!!" << std::endl;
		for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); planet_id++) {
			auto building = game_wrapper.getBuilding(planet_id);
			if (building.has_value())
				addTask(new DestroyTask(planet_id, 10, Specialty::PRODUCTION), 0, game_wrapper);
		}
	};
};
