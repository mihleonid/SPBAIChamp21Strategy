#include <algorithm>
#include "../graph.h"
#include "../Core.h"
#include "../tasks/MoveRobotsTask.h"

void Core::defendLogic(int priority, GameWrapper& game_wrapper) {
	std::unordered_map<int, double> endangered_planets;

	for (FlyingWorkerGroup flying_worker_group : game_wrapper.getFlyingWorkerGroups()) {
		// Если старая группа, то мы уже среагировали
		if (flying_worker_group.departureTick != game_wrapper.getCurrentTick()) continue;
		if (game_wrapper.isPlayerFriend(flying_worker_group.playerIndex))
			continue;

		if (std::find_if(building_locations.begin(), building_locations.end(),
						[flying_worker_group](const std::pair<BuildingType, std::set<int>>& entry)
							{ return entry.second.find(flying_worker_group.targetPlanet) != entry.second.end(); }) !=
						 building_locations.end()) {
			double strength = flying_worker_group.number;
			std::optional<Specialty> specialty = game_wrapper.getPlayerSpecialty(flying_worker_group.playerIndex);
			if (specialty.has_value() && specialty == Specialty::COMBAT)
				strength += strength * game_wrapper.getCombatUpgrade() / 100.0;
			endangered_planets[flying_worker_group.targetPlanet] += strength;
		}
	}

	for (const auto&[planet_id, enemy_strength] : endangered_planets) {
		std::set<std::pair<int, int>> other_planets;
		for (const auto&[resource, locations]: building_locations) {
			for (int building_planet_id: locations) {
				if (planet_id == building_planet_id)
					continue;
				int dist = 0;
				for (Specialty specialty : {Specialty::COMBAT, Specialty::LOGISTICS, Specialty::PRODUCTION})
					dist += Graph::getInstance()->distBySpecialty(building_planet_id, planet_id, specialty);
				other_planets.insert({dist, building_planet_id});
			}
		}
		for (std::pair<int, int> dist_and_planet : other_planets) {
			int building_planet_id = dist_and_planet.second;
			if (game_wrapper.getMyTeamFreeRobotCount(building_planet_id) > 0) {
				for (Specialty specialty: {Specialty::COMBAT, Specialty::LOGISTICS, Specialty::PRODUCTION}) {
					int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
					if (player_id != -1) {
						int packet = game_wrapper.getFreeRobotCount(building_planet_id, player_id) / 3; // magic number

						if (specialty == Specialty::COMBAT)
							packet = (int)std::min((double)packet, endangered_planets[planet_id] / (1.0 + game_wrapper.getCombatUpgrade() / 100.0));
						else
							packet = (int)std::min((double)packet, endangered_planets[planet_id]);
						if (packet > 0) {
							// std::cerr << "Defending planet #" << planet_id << " from #" << building_planet_id << std::endl;
							addTask(new MoveRobotsTask(building_planet_id, planet_id, packet, specialty),
									priority, game_wrapper);
							endangered_planets[planet_id] -= packet;
							if (specialty == Specialty::COMBAT)
								endangered_planets[planet_id] -= packet * game_wrapper.getCombatUpgrade() / 100.0;
						}
					}
				}
			}
		}
	}
}
