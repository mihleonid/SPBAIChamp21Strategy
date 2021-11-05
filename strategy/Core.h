#pragma once
#include <iostream>
#include <map>
#include <unordered_set>
#include <functional>
#include "GameWrapper.h"
#include "tasks/Task.h"
#include "tasks/DestroyTask.h"
#include "tasks/BuildTask.h"
#include "graph.h"

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
		&Core::buildLogic,
		&Core::supplyingLogistics,
		&Core::workAssignment,
		&Core::returnLogistics
	};
	/*
	 *  Битва
	 *	Разрушение
	 *	Строительство
	 *	Перемещение на атаку
	 *	Перемещение на защиту (?)
	 *	Перенос ресурсов на потребителя
	 *	Производство
	 */

	std::unordered_map<BuildingType, std::vector<int>> building_locations;
	std::unordered_map<Resource, std::unordered_set<int>> dependencies;

	void selectPlanets(const GameWrapper& game_wrapper);

	void destroyLogic(int priority, GameWrapper& game_wrapper);

	void buildLogic(int priority, GameWrapper& game_wrapper);

	void updateDependencies(const GameWrapper &game_wrapper);
	void supplyingLogistics(int priority, GameWrapper& game_wrapper);
	void workAssignment(int priority, GameWrapper& game_wrapper);
	void returnLogistics(int priority, GameWrapper& game_wrapper);
};
