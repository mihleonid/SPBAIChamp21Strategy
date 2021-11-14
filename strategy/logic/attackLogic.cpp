#include "../Core.h"
#include "../tasks/MoveRobotsTask.h"
#include "algorithm"

void Core::attackLogic(int priority, GameWrapper& game_wrapper) {
	static bool is_attacking = false;
	std::set<std::pair<int, int>> enemy_planets;
	if (game_wrapper.getOurTotalBattlePower() > game_wrapper.getEnemyTotalBattlePower() + 250 || is_attacking) { // magic number
		is_attacking = true;
		for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); ++planet_id) {
			int enemies = 0;
			int my = 0;

			for (Specialty specialty : {Specialty::COMBAT, Specialty::PRODUCTION, Specialty::LOGISTICS}) {
				enemies += game_wrapper.getEnemyFreeRobotCount(planet_id, specialty);
				my += game_wrapper.getMyFreeRobotCount(planet_id, specialty);

				if (specialty == Specialty::COMBAT) {
					enemies += game_wrapper.getEnemyFreeRobotCount(planet_id, specialty) * game_wrapper.getCombatUpgrade() / 100;
					my += game_wrapper.getMyFreeRobotCount(planet_id, specialty) * game_wrapper.getCombatUpgrade() / 100;
				}
			}

			if (enemies > my) {
				enemy_planets.insert({enemies - my, planet_id});
				std::cout << "Enemies: " << planet_id << std::endl;
			}
		}
	}

	for (auto it = enemy_planets.rbegin(); it != enemy_planets.rend(); ++it) {
		std::set<std::pair<int, int>> nearest_planets;
		for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); ++planet_id) {
			nearest_planets.insert({Graph::getInstance()->distBySpecialty(planet_id, it->second, Specialty::COMBAT), planet_id});
		}

		int remain = it->first;

		for (Specialty specialty : {Specialty::COMBAT, Specialty::LOGISTICS, Specialty::PRODUCTION}) {
			for (const auto& [distance, from] : nearest_planets) {
				int robots = game_wrapper.getMyFreeRobotCount(from, specialty);
				int to_send = std::min(remain, robots);
				if (to_send > 0) {
					addTask(new MoveRobotsTask(from, it->second, to_send, specialty), priority, game_wrapper);
					remain -= to_send;
				}
			}
		}
	}
}

