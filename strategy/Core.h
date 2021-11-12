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
		// &Core::destroyLogic,
		&Core::establishingBuildings,
		&Core::productionLogic,
		&Core::abandonLogic
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
	// needed_resource -> {location -> amount}
	// amount = необходимое - (уже есть на планете + в пути)
	std::unordered_map<Resource, std::unordered_map<int, int>> required_resources;
	std::unordered_map<Resource, std::unordered_set<int>> dependencies;

	void selectPlanets(const GameWrapper& game_wrapper);

	void destroyLogic(int priority, GameWrapper& game_wrapper);

	void buildLogic(int priority, GameWrapper& game_wrapper);

	void updateDependencies(const GameWrapper &game_wrapper);

	inline void productionLogic(int priority, GameWrapper& game_wrapper) {
		supplyingLogistics(priority, game_wrapper);
		workAssignment(priority, game_wrapper);
		returnLogistics(priority, game_wrapper);
	}

	inline void establishingBuildings(int priority, GameWrapper& game_wrapper) {
		buildLogic(priority, game_wrapper);
		deliveryLogic(priority, game_wrapper);
	}

	void supplyingLogistics(int priority, GameWrapper& game_wrapper);
	void workAssignment(int priority, GameWrapper& game_wrapper);
	void returnLogistics(int priority, GameWrapper& game_wrapper);
	void abandonLogic(int priority, GameWrapper &game_wrapper);
	void deliveryLogic(int priority, GameWrapper &game_wrapper);

	const std::unordered_map<Resource, BuildingType> resource_to_building_type = {
		{Resource::STONE, BuildingType::QUARRY},
		{Resource::ORE, BuildingType::MINES},
		{Resource::ORGANICS, BuildingType::FARM},
		{Resource::SAND, BuildingType::CAREER},

		{Resource::ACCUMULATOR, BuildingType::ACCUMULATOR_FACTORY},
		{Resource::CHIP, BuildingType::CHIP_FACTORY},
		{Resource::METAL, BuildingType::FOUNDRY},
		{Resource::PLASTIC, BuildingType::BIOREACTOR},
		{Resource::SILICON, BuildingType::FURNACE}
	};
};
