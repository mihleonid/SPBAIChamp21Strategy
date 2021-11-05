#include "../Core.h"

const static std::map<BuildingType, std::vector<std::pair<BuildingType, int>>> BUILDING_DEPENDENCIES = {
	{BuildingType::FARM, {{BuildingType::QUARRY, 1}}},
	{BuildingType::CAREER, {{BuildingType::FARM, 1}}},
	{BuildingType::MINES, {{BuildingType::FARM, 1}}},

	{BuildingType::FURNACE, {{BuildingType::CAREER, 1}}},
	{BuildingType::BIOREACTOR, {{BuildingType::FARM, 1}}},
	{BuildingType::FOUNDRY, {{BuildingType::MINES, 1}}},

	{BuildingType::ACCUMULATOR_FACTORY, {{BuildingType::FOUNDRY, 1}, {BuildingType::BIOREACTOR, 1}}},
	{BuildingType::CHIP_FACTORY, {{BuildingType::FOUNDRY, 1}, {BuildingType::FURNACE, 1}}},

	{BuildingType::REPLICATOR, {{BuildingType::FOUNDRY, 2}, {BuildingType::ACCUMULATOR_FACTORY, 1}, {BuildingType::CHIP_FACTORY, 2}}}
};

void Core::selectPlanets(const GameWrapper& game_wrapper) { // select my planets
	std::vector<bool> used(game_wrapper.getPlanetsCount());

	for (int player_id = 0; player_id < 6; ++player_id) {
		if (game_wrapper.isPlayerFriend(player_id)) {
			building_locations[BuildingType::QUARRY].push_back(game_wrapper.getPlayerStartingPlanet(player_id));
			used[game_wrapper.getPlayerStartingPlanet(player_id)] = true;
		}
	}

	for (const auto& [building_type, dependence] : BUILDING_DEPENDENCIES) {
		BuildingProperties building_property = game_wrapper.getBuildingProperties(building_type);
		std::vector<std::pair<int, int>> dependencies; // planet_id weight
		for (const auto& [pf, weight] : dependence) {
			for (int planet_id : building_locations[pf]) {
				dependencies.push_back({planet_id, weight});
			}
		}

		int best_variant = -1, best_dist = -1;
		for (int planet_id = 0; planet_id < game_wrapper.getPlanetsCount(); ++planet_id) {
			if (used[planet_id])
				continue;
			int dst = 0;
			for (const auto& [dependence_planet_id, weight] : dependencies) {
				dst +=
					(Graph::getInstance()->distBySpecialty(planet_id, dependence_planet_id, Specialty::COMBAT) +
						Graph::getInstance()->distBySpecialty(planet_id, dependence_planet_id, Specialty::LOGISTICS) +
						Graph::getInstance()->distBySpecialty(planet_id, dependence_planet_id, Specialty::PRODUCTION)) * weight;
			}

			if (best_variant == -1 || dst < best_dist) {
				best_dist = dst;
				best_variant = planet_id;
			}
		}

		used[best_variant] = true;
		building_locations[building_type].push_back(best_variant);
	}
}

void Core::buildLogic(int priority, GameWrapper &game_wrapper) {
	if (building_locations.empty())
		selectPlanets(game_wrapper);

	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		for (int planet_id : locations) {
			auto building = game_wrapper.getBuilding(planet_id);
			if (building.has_value() && building.value().health == info.maxHealth)
				continue;

			if (game_wrapper.isEnoughResourcesToBuild(planet_id, building_type) ||
				(building.has_value() && building->health < info.maxHealth)) {
				int player_id = game_wrapper.getMyPlayerIdBySpecialty(Specialty::COMBAT);
				if (!building.has_value() && player_id != -1)
					addTask(new BuildTask(planet_id, BuildingType::QUARRY, 10, Specialty::COMBAT), priority, game_wrapper);
			}
		}
	}
}
