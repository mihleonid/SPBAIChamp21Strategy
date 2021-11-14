#include "../Core.h"

void Core::destroyLogic(int priority, GameWrapper& game_wrapper) {
	for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); planet_id++) {
		if (std::find_if(building_locations.begin(), building_locations.end(), [planet_id](const std::pair<BuildingType, std::set<int>>& locations)
			{return locations.second.find(planet_id) != locations.second.end();}) != building_locations.end())
			continue;

		auto building = game_wrapper.getBuilding(planet_id);
		if (building.has_value()) {
			for (Specialty specialty : {Specialty::COMBAT, Specialty::PRODUCTION, Specialty::LOGISTICS}) {
				if (game_wrapper.getMyFreeRobotCount(planet_id, specialty) > 0)
					addTask(new DestroyTask(planet_id, game_wrapper.getMyFreeRobotCount(planet_id, specialty),
											specialty), priority, game_wrapper);
			}
		}
	}
};
