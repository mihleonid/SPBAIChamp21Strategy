#include "../Core.h"
#include <unordered_map>
#include <algorithm>
#include <random>
#include <strategy/tasks/MoveRobotsTask.h>

#include "../tasks/MoveResourceTask.h"
#include "../tasks/WorkTask.h"

std::unordered_map<Resource, BuildingType> resource_to_building_type = {
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

void Core::updateDependencies(const GameWrapper& game_wrapper) {
	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		for (int planet_id : locations) {
			auto building = game_wrapper.getBuilding(planet_id);
			for (const auto&[needed_resource, amount] : info.buildResources)
				if (!building.has_value()) {
					std::cerr << "Building on the planet #" << planet_id << " is not built!" << std::endl;
					dependencies[needed_resource].insert(planet_id);
				} else {
					dependencies[needed_resource].erase(planet_id);
				}
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
					} else {
						std::cerr << "Too many resources on planet #" << consumer << std::endl;
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
			// Работа
			int can_produce = 10e9;
			for (const auto&[neededResource, amount]: info.workResources) {
				can_produce = std::min(can_produce, game_wrapper.getResourceCount(planet_id, neededResource) / amount);
			}
			int working_robots = std::min(can_produce, info.maxWorkers);
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
			for (const auto&[needed_resource, planets] : dependencies) {
				// Находим, от чего зависит здание
				if (planets.find(planet_id) == planets.end())
					continue;

				BuildingType supplier_type = resource_to_building_type.at(needed_resource);
				for (int supplier_planet : building_locations[supplier_type])
					suppliers.push_back(supplier_planet);
			}
			if (suppliers.size() == 0) continue;

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
