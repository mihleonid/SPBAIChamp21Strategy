#include "../Core.h"
#include <unordered_map>
#include <algorithm>
#include <random>
#include <strategy/tasks/MoveRobotsTask.h>

#include "../tasks/MoveResourceTask.h"
#include "../tasks/WorkTask.h"

void Core::updateDependencies(const GameWrapper& game_wrapper) {
	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		for (int planet_id : locations) {
			// auto building = game_wrapper.getBuilding(planet_id);
			// for (const auto&[needed_resource, amount] : info.buildResources)
				/*if (!building.has_value()) {
					std::cerr << "Building on the planet #" << planet_id << " is not built!" << std::endl;
					if (required_resources[needed_resource][planet_id] > 0)
						dependencies[needed_resource].insert(planet_id);
					else dependencies[needed_resource].erase(planet_id);
				} else {
					dependencies[needed_resource].erase(planet_id);
				}*/
			for (const auto&[needed_resource, amount] : info.workResources) {
				dependencies[needed_resource].insert(planet_id);
			}
		}
	}
}

void Core::supplyingLogistics(int priority, GameWrapper &game_wrapper) {
	updateDependencies(game_wrapper);
	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		for (int planet_id : locations) {
			// Вперед
			if (info.produceResource.has_value() && !dependencies[info.produceResource.value()].empty()) {
				int produced_resource = std::min(game_wrapper.getResourceCount(planet_id, info.produceResource.value()),
												 game_wrapper.getMyTeamFreeRobotCount(planet_id));
				// Ищем, какое кол-во ресурса мы можем оставить на планетах - потребителях
				int l = 0, r = 1e9, goal_number;
				while (r - l > 1) {
					goal_number = (l + r) / 2;
					int total_send = 0;
					for (int consumer : dependencies[info.produceResource.value()]) {
						total_send += std::max(goal_number - game_wrapper.getResourceCount(consumer, info.produceResource.value()), 0);
					}
					if (total_send <= produced_resource)
						l = goal_number;
					else
						r = goal_number;
				}
				// Нашли
				goal_number = l;

				for (int consumer : dependencies[info.produceResource.value()]) {
					int to_send = goal_number - game_wrapper.getResourceCount(consumer, info.produceResource.value());
					if (to_send > 0) {
						// Распределяем по специальностям
						for (Specialty specialty : {Specialty::LOGISTICS, Specialty::COMBAT, Specialty::PRODUCTION}) {
							int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
							if (player_id == -1) {
								std::cerr << (int)specialty << " have not been assigned!" << std::endl;
							} else {
								int robot_cnt = game_wrapper.getFreeRobotCount(planet_id, player_id);
								int packet = std::min(to_send, robot_cnt);
								if (packet > 0) {
									addTask(new MoveResourceTask(
												planet_id, consumer,
												info.produceResource.value(), packet,
												specialty),
											priority, game_wrapper);
									to_send -= packet;
								}
							}
						}
					}
				}
			}
		}
	}
}

void Core::workAssignment(int priority, GameWrapper &game_wrapper) {
	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		for (int planet_id: locations) {
			/*if (building_type == model::BuildingType::QUARRY) {
				bool required_stone = false;
				for (const auto& [_, amount] : required_resources[model::Resource::STONE]) {
					if (amount > 0) {
						required_stone = true;
						break;
					}
				}
				if (required_stone) {
					for (Specialty specialty: {Specialty::PRODUCTION, Specialty::COMBAT, Specialty::LOGISTICS}) {
						std::cerr << "Working on quarry " << game_wrapper.getMyFreeRobotCount(planet_id, specialty) << std::endl;
						addTask(new WorkTask(planet_id, game_wrapper.getMyFreeRobotCount(planet_id, specialty), specialty),
								priority, game_wrapper);
					}
				}
			}*/
			// Если производим то, что никому не нужно
			if (info.produceResource.has_value() && dependencies[info.produceResource.value()].empty()) break;

			// Работа
			int can_produce = 1e9;
			for (const auto&[neededResource, amount]: info.workResources) {
				can_produce = std::min(can_produce, game_wrapper.getResourceCount(planet_id, neededResource) / amount);
			}

			//FIXME найти лучшее соотношение для следующей формулы. Пока тесты показали: это лучшее. Но может мы не все варианты пробовали?
			int working_robots = can_produce; //FIXME std::min(can_produce, info.maxWorkers);
			//int working_robots = can_produce * info.workAmount; //FIXME std::min(can_produce, info.maxWorkers);
			// Распределяем по специальностям
			for (Specialty specialty : {Specialty::PRODUCTION, Specialty::COMBAT, Specialty::LOGISTICS}) {
				int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
				if (player_id == -1) {
					std::cerr << (int)specialty << " have not been assigned!" << std::endl;
				} else {
					int robot_cnt = game_wrapper.getFreeRobotCount(planet_id, player_id);
					int packet = std::min(working_robots, robot_cnt);
					if (packet > 0) {
						addTask(new WorkTask(planet_id, packet, specialty),
								priority, game_wrapper);
						working_robots -= packet;
					}
				}
			}
		}
	}
}

void Core::returnLogistics(int priority, GameWrapper &game_wrapper) {
	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		for (int planet_id : locations) {
			std::vector<int> suppliers;

			for (const auto&[needed_resource, amount] : info.workResources) {
				BuildingType supplier_type = resource_to_building_type.at(needed_resource);

				auto it = building_locations[supplier_type].begin();
				std::advance(it, (int)rand() % building_locations[supplier_type].size());
				suppliers.push_back(*it);
			}

			if (suppliers.empty()) continue;

			int supplier_id = (int)rand() % suppliers.size();
			int to_send = game_wrapper.getMyTeamFreeRobotCount(planet_id);
			// Распределяем по специальностям
			for (Specialty specialty : {Specialty::LOGISTICS, Specialty::COMBAT, Specialty::PRODUCTION}) {
				int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
				if (player_id == -1) {
					std::cerr << (int)specialty << " have not been assigned!" << std::endl;
				} else {
					int robot_cnt = game_wrapper.getFreeRobotCount(planet_id, player_id);
					int packet = std::min(to_send, robot_cnt);
					if (packet > 0) {
						addTask(new MoveRobotsTask(
									planet_id, suppliers[supplier_id],
									packet,
									specialty),
								priority, game_wrapper);
						to_send -= packet;
					}
				}
			}
		}
	}
}

void Core::send_to_replicator(int planet_id, int priority, GameWrapper &game_wrapper) {
	std::set<int> replicators = building_locations[BuildingType::REPLICATOR];
	if (!replicators.empty()) {
		auto it = replicators.begin();
		std::advance(it, (int) rand() % replicators.size());
		int random_replicator = *it;
		for (Specialty sp: {Specialty::LOGISTICS, Specialty::COMBAT, Specialty::PRODUCTION}) {
			if (game_wrapper.getCurrentTick() > 5) { // magic number
				addTask(new MoveRobotsTask(
					planet_id, random_replicator,
					game_wrapper.getMyFreeRobotCount(planet_id, sp),
					sp
				), priority, game_wrapper);
			}
		}
	}
}

void Core::abandonLogic(int priority, GameWrapper &game_wrapper) {
	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		// Если мы ничего не производим, то мы нужны для чего-то другого
		if (!info.produceResource.has_value()) continue;
		// проверяем, что мы не нужны для строительства чего-либо
		const auto& my_requires = required_resources[info.produceResource.value()];
		bool is_needed_for_building = std::find_if(my_requires.begin(),
												   my_requires.end(),
												   [](const std::pair<int, int>& p) { return p.second > 0; }) != my_requires.end();
		if (is_needed_for_building) continue;

		for (int planet_id: locations) {
			// проверяем, что наш ресурс нужен на производстве
			if (info.produceResource.has_value() && dependencies[info.produceResource.value()].empty()) {
				//std::cerr << "Abandoned: " << planet_id << std::endl;
				// мы не нужны(
				// отправляем роботов на репликатор
				send_to_replicator(planet_id, priority, game_wrapper);
			}
		}
	}
/*
	for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); ++planet_id) {
		if (game_wrapper.getMyTeamFreeRobotCount(planet_id) > 0) {
			send_to_replicator(planet_id, priority, game_wrapper);
		}
	}*/
}
