#pragma once
#include <iostream>
#include <map>
#include <unordered_set>
#include <functional>
#include "GameWrapper.h"
#include "tasks/Task.h"
#include "tasks/DestroyTask.h"
#include "tasks/BuildTask.h"

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
		&Core::destroyLogic,
		&Core::buildLogic
	};
	/*
	 * MoveResourceTask
		MoveRobotsTask
		BuildTask
		DestroyTask
		WaitingTask
		WorkTask

	 */
	inline void destroyLogic(int priority, GameWrapper& game_wrapper) {
		std::cout << "DESTROY!" << std::endl;
		for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); planet_id++) {
			auto building = game_wrapper.getBuilding(planet_id);
			int player_id = game_wrapper.getMyPlayerIdBySpecialty(Specialty::PRODUCTION);
			if (building.has_value() && player_id != -1)
				addTask(new DestroyTask(planet_id, 10, Specialty::PRODUCTION), priority, game_wrapper);
		}
	};

	inline void buildLogic(int priority, GameWrapper& game_wrapper) {
		std::cout << "BUILD!" << std::endl;
		for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); planet_id++) {
			auto building = game_wrapper.getBuilding(planet_id);
			int player_id = game_wrapper.getMyPlayerIdBySpecialty(Specialty::PRODUCTION);
			if (!building.has_value() && player_id != -1)
				addTask(new BuildTask(planet_id, BuildingType::QUARRY, 10, Specialty::PRODUCTION), priority, game_wrapper);
		}
	}
};
