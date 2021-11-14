#include <algorithm>
#include "../Core.h"

const static std::vector<std::pair<BuildingType, std::vector<std::pair<BuildingType, int>>>> BUILDING_DEPENDENCIES = {
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

const static std::map<BuildingType, int> BUILDING_COUNT = {
	{BuildingType::REPLICATOR, 1},
	{BuildingType::CHIP_FACTORY, 1},
	{BuildingType::FURNACE, 2},
	{BuildingType::CAREER, 1},
	{BuildingType::FOUNDRY, 2},
	{BuildingType::MINES, 2},
	{BuildingType::ACCUMULATOR_FACTORY, 1},
	{BuildingType::BIOREACTOR, 1},
	{BuildingType::FARM, 1}
};

void Core::requestBuilding(BuildingType building_type, int planet_id, const GameWrapper& game_wrapper) {
	BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
	for (const auto&[needed_resource, amount] : info.buildResources) {
		required_resources[needed_resource][planet_id] = amount;
	}
	building_locations[building_type].insert(planet_id);
}

void Core::selectPlanets(const GameWrapper& game_wrapper) { // select my planets
	for (int player_id = 0; player_id < 6; ++player_id) {
		if (game_wrapper.isPlayerFriend(player_id)) {
			building_locations[BuildingType::QUARRY].insert(game_wrapper.getPlayerStartingPlanet(player_id));
		}
	}

	for (const auto& [building_type, dependence] : BUILDING_DEPENDENCIES) {
		// for (int cnt = 0; cnt < BUILDING_COUNT.at(building_type); ++cnt) {
		while (building_locations[building_type].size() < BUILDING_COUNT.at(building_type)) {
			BuildingProperties building_property = game_wrapper.getBuildingProperties(building_type);
			std::vector<std::pair<int, int>> dependencies; // planet_id weight
			for (const auto& [pf, weight] : dependence) {
				for (int planet_id : building_locations[pf]) {
					dependencies.emplace_back(planet_id, weight);
				}
			}

			int best_variant = -1, best_dist = -1;
			for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); ++planet_id) {
				const Planet& planet = game_wrapper.getPlanets()[planet_id];
				if (std::find_if(building_locations.begin(),
								building_locations.end(),
								[planet_id](const std::pair<BuildingType, std::set<int>>& v)
									{return v.second.find(planet_id) != v.second.end();}) != building_locations.end() ||
				(building_property.harvest && planet.harvestableResource != building_property.produceResource))
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
			requestBuilding(building_type, best_variant, game_wrapper);
		}
	}
}

void Core::buildLogic(int priority, GameWrapper &game_wrapper) {
	if (building_locations.empty())
		selectPlanets(game_wrapper);

	for (int planet_id : building_locations[BuildingType::QUARRY]) {
		auto building = game_wrapper.getBuilding(planet_id);
		if (building.has_value() && game_wrapper.getResourceCount(planet_id, Resource::STONE) > 0) {
			game_wrapper.reserveResources(planet_id, Resource::STONE, std::min(250, game_wrapper.getResourceCount(planet_id, Resource::STONE)));
		}
	}

	for (const auto&[building_type, locations] : building_locations) {
		BuildingProperties info = game_wrapper.getBuildingProperties(building_type);
		for (int planet_id : locations) {
			auto building = game_wrapper.getBuilding(planet_id);

			if (building.has_value() && building.value().buildingType != building_type) {
				for (Specialty specialty : {Specialty::COMBAT, Specialty::PRODUCTION, Specialty::LOGISTICS}) {
					int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
					if (player_id != -1) {
						if (game_wrapper.getRobotCount(planet_id, player_id) > 0) {
							addTask(new DestroyTask(planet_id, game_wrapper.getRobotCount(planet_id, player_id), specialty), priority, game_wrapper);
							break;
						}
					}
				}
			}

			if (building.has_value() && building.value().health == info.maxHealth) {
				continue;
			}

			if (!building.has_value() && building_exists[planet_id]) {
				requestBuilding(building_type, planet_id, game_wrapper);
				building_exists[planet_id] = false;
			}

			if (game_wrapper.isEnoughResourcesToBuild(planet_id, building_type) ||
				(building.has_value() && building->health < info.maxHealth)) {
				// Выбираем специализацию для постройки
				for (Specialty specialty : {Specialty::COMBAT, Specialty::PRODUCTION, Specialty::LOGISTICS}) {
					int player_id = game_wrapper.getMyPlayerIdBySpecialty(specialty);
					if (!building.has_value() && player_id != -1) {
						if (game_wrapper.getRobotCount(planet_id, player_id) > 0) {
							building_exists[planet_id] = true;
							addTask(new BuildTask(planet_id, building_type,
												  game_wrapper.getRobotCount(planet_id, player_id),
												  specialty), priority, game_wrapper);
							break;
						}
					}
				}
			}
		}
	}
}
