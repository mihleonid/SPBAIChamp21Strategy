#include <algorithm>
#include "../Core.h"

void Core::battleLogic(int priority, GameWrapper& game_wrapper) {
	for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); ++planet_id) {
		int robot_cnt = game_wrapper.getMyTeamRobotCount(planet_id) +
			(game_wrapper.getMyRobotCount(planet_id, Specialty::COMBAT) * game_wrapper.getCombatUpgrade() / 100);
		if (robot_cnt == 0) continue;
		int enemy_cnt = game_wrapper.getEnemyTeamRobotCount(planet_id) +
			(game_wrapper.getEnemyRobotCount(planet_id, Specialty::COMBAT) * game_wrapper.getCombatUpgrade() / 100);
		// std::cerr << "Planet: #" << planet_id << ' ' << enemy_cnt << std::endl;

		for (Specialty specialty : {Specialty::COMBAT, Specialty::LOGISTICS, Specialty::PRODUCTION}) {
			int specialty_cnt = game_wrapper.getMyRobotCount(planet_id, specialty);
			if (specialty == model::Specialty::COMBAT) specialty_cnt += (specialty_cnt * game_wrapper.getCombatUpgrade() / 100);

			int specialty_dmg = enemy_cnt * ((specialty_cnt + robot_cnt - 1) / robot_cnt);

			if (specialty_dmg > 0)
				game_wrapper.reserveMyRobots(planet_id, specialty, std::min(specialty_dmg, game_wrapper.getMyRobotCount(planet_id, specialty)));
		}
	}
}

