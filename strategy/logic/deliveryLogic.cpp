#include "../Core.h"
#include <utility>
#include <set>
#include <algorithm>
#include <chrono>
#include "../tasks/MoveResourceTask.h"
#include "../tasks/MoveRobotsTask.h"

void Core::deliveryLogic(int priority, GameWrapper& game_wrapper) {
	for (const auto&[needed_resource, locations]: required_resources) {
		// std::cout << resourceToString(needed_resource) << " is required by:" << std::endl;

		for (const auto&[planet_id, amount]: locations) {
			std::set<std::pair<int, int>> sources; // {amount, planet_id}
			for (int source_id : building_locations[resource_to_building_type.at(needed_resource)]) {
				sources.emplace(game_wrapper.getResourceCount(source_id, needed_resource), source_id);
			}
			// std::cout << planet_id << ' ' << amount << std::endl;
			if (amount > 0) {
				int remain = amount;
				for (auto it = sources.rbegin(); it != sources.rend() && remain > 0; it++) {
					// Выбираем специализацию для отправки
					for (Specialty specialty : {Specialty::LOGISTICS, Specialty::COMBAT, Specialty::PRODUCTION}) {
						int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
						if (player_id != -1) {
							int packet = std::min(
								{
									game_wrapper.getFreeRobotCount(it->second, player_id),
									amount, it->first
								});
							if (packet > 0) {
								auto *new_task = new MoveResourceTask(it->second, planet_id, needed_resource, packet, specialty);
								new_task->setOnDone([this](const MoveResourceTask& move_resource_task) {
									required_resources[move_resource_task.getResource()]
									[move_resource_task.getArrivalPlanet()] += move_resource_task.getPacketLoss();
								});
								required_resources[needed_resource][planet_id] -= packet;
								remain -= packet;
								addTask(new_task, priority, game_wrapper);
							}
						}
					}
				}

				if (remain > 0) {
					int min_dist = 1e9;
					int min_id = 0;
					for (auto it = sources.begin(); it != sources.end(); ++it) {
						int dist = Graph::getInstance()->distBySpecialty(planet_id, it->second, Specialty::COMBAT);
						if (dist < min_dist) {
							min_dist = dist;
							min_id = it->second;
						}
					}
					// Выбираем специализацию для отправки
					for (Specialty specialty : {Specialty::LOGISTICS, Specialty::PRODUCTION, Specialty::COMBAT}) {
						int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
						if (player_id != -1) {
							int packet = std::min(game_wrapper.getFreeRobotCount(planet_id, player_id), remain);
							if (packet > 0) {
								addTask(new MoveRobotsTask(planet_id, min_id, packet, specialty), priority, game_wrapper);
								remain -= packet;
							}
						}
					}
				}
			}
		}
		std::cout<<std::endl;
	}
}
