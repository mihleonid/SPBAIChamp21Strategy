#include "GameWrapper.h"

#include <algorithm>
#include <unordered_set>

void GameWrapper::update(const Game &g) {
	game = &g;

	if (player_starting_planet.empty()) {
		// init
		for (int planet_id = 0; planet_id < game->planets.size(); ++planet_id) {
			if (game->planets[planet_id].building.has_value()) {
				player_starting_planet[game->planets[planet_id].workerGroups[0].playerIndex] = planet_id;
			}
		}

		/*int i = 0;
		for (int player_id = 0; player_id < 6; ++player_id) {
			if (isPlayerFriend(player_id)) {
				my_specialities[player_id] = (Specialty)i++;
			}
		}*/
	}

	free_robots.resize(game->planets.size());
	free_resources.resize(game->planets.size());

	for (int planet_id = 0; planet_id < game->planets.size(); ++planet_id) {
		free_robots[planet_id].clear();
		free_resources[planet_id].clear();

		for (const WorkerGroup &worker_group: game->planets[planet_id].workerGroups) {
			free_robots[planet_id][worker_group.playerIndex] += worker_group.number;
		}

		for (const auto&[resource, amount]: game->planets[planet_id].resources) {
			free_resources[planet_id][resource] += amount;
		}
	}

	for (int player_id = 0; player_id < game->players.size(); ++player_id) {
		int current_flying_groups =
			std::count_if(game->flyingWorkerGroups.begin(),
						  game->flyingWorkerGroups.end(),
						  [player_id](const FlyingWorkerGroup &group) { return group.playerIndex == player_id; });
		available_flying_groups[player_id] = game->maxFlyingWorkerGroups - current_flying_groups;
	}

	for (int planet_id = 0; planet_id < game->planets.size(); ++planet_id) {
		if (game->planets[planet_id].building.has_value()) {
			Building building = game->planets[planet_id].building.value();
			if (building.health == game->buildingProperties.at(building.buildingType).maxHealth) {
				free_worker_place[planet_id] = game->buildingProperties.at(building.buildingType).maxWorkers;
			} else {
				free_worker_place[planet_id] = 0;
			}
		} else {
			free_worker_place[planet_id] = 0;
		}
	}
}

int GameWrapper::getRobotCount(int planet_id, int player_id) const {
	int robots = 0;
	for (const WorkerGroup &worker_group: game->planets[planet_id].workerGroups) {
		if (worker_group.playerIndex == player_id) {
			robots += worker_group.number;
		}
	}
	return robots;
}

int GameWrapper::getFreeRobotCount(int planet_id, int player_id) const {
	if (free_robots[planet_id].find(player_id) != free_robots[planet_id].end())
		return free_robots[planet_id].at(player_id);
	return 0;
}

int GameWrapper::getMyTeamRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < game->players.size(); ++i) {
		if (isPlayerFriend(i))
			res += getRobotCount(planet_id, i);
	}
	return res;
}

int GameWrapper::getMyTeamFreeRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < game->players.size(); ++i) {
		if (isPlayerFriend(i))
			res += getFreeRobotCount(planet_id, i);
	}
	return res;
}

int GameWrapper::getEnemyTeamRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < game->players.size(); ++i) {
		if (isPlayerEnemy(i))
			res += getRobotCount(planet_id, i);
	}
	return res;
}

int GameWrapper::getEnemyTeamFreeRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < game->players.size(); ++i) {
		if (isPlayerEnemy(i))
			res += getFreeRobotCount(planet_id, i);
	}
	return res;
}

int GameWrapper::getMyTotalRobotCount() const {
	int res = 0;
	for (int i = 0; i < game->planets.size(); ++i)
		res += getMyTeamRobotCount(i);
	return res;
}
int GameWrapper::getMyTotalFreeRobotCount() const {
	int res = 0;
	for (int i = 0; i < game->planets.size(); ++i)
		res += getMyTeamFreeRobotCount(i);
	return res;
}

int GameWrapper::getEnemyTotalRobotCount() const {
	int res = 0;
	for (int i = 0; i < game->planets.size(); ++i)
		res += getEnemyTeamRobotCount(i);
	return res;
}
int GameWrapper::getEnemyTotalFreeRobotCount() const {
	int res = 0;
	for (int i = 0; i < game->planets.size(); ++i)
		res += getEnemyTeamFreeRobotCount(i);
	return res;
}

int GameWrapper::getMyTeamScore() const {
	int res = 0;
	for (int i = 0; i < game->players.size(); ++i) {
		if (getPlayerTeamId(i) == getMyTeamId()) {
			res += game->players[i].score;
		}
	}
	return res;
}
int GameWrapper::getEnemyTeamScore() const {
	int result = 0;
	for (int player_id = 0; player_id < game->players.size(); ++player_id) {
		if (isPlayerEnemy(player_id)) {
			result += game->players[player_id].score;
		}
	}
	return result;
}

bool GameWrapper::isPlayerEnemy(int player_id) const {
	return getMyTeamId() != getPlayerTeamId(player_id);
}

bool GameWrapper::isPlayerFriend(int player_id) const {
	return getMyTeamId() == getPlayerTeamId(player_id);
}

std::unordered_map<Resource, int> GameWrapper::getResourcesCount(int planet_id) const {
	return free_resources[planet_id];
}

int GameWrapper::getResourceCount(int planet_id, Resource resource) const {
	if (free_resources[planet_id].find(resource) != free_resources[planet_id].end())
		return free_resources[planet_id].at(resource);
	return 0;
}

int GameWrapper::getEnemyTeamId() const {
	for (int player_id = 0; player_id < game->players.size(); ++player_id) {
		if (isPlayerEnemy(player_id)) {
			return getPlayerTeamId(player_id);
		}
	}
	return -1;
}

bool GameWrapper::isEnoughResourcesToBuild(int planet_id, BuildingType building_type) const {
	BuildingProperties info = getBuildingProperties(building_type);
	std::unordered_map<Resource, int> resources = getResourcesCount(planet_id);
	for (auto const&[neededResource, amount]: info.buildResources) {
		if (resources[neededResource] < amount)
			return false;
	}
	return true;
}

int GameWrapper::reserveRobots(int planet_id, int player_id, int cnt) {
	if (free_robots[planet_id][player_id] >= cnt) {
		// Successful reservation
		free_robots[planet_id][player_id] -= cnt;
		return cnt;
	}
	return free_robots[planet_id][player_id];
}

int GameWrapper::reserveResources(int planet_id, Resource resource, int cnt) {
	// assert(free_resources[planetId][resource]>=cnt);
	if (free_resources[planet_id][resource] >= cnt) {
		// Successful reservation
		free_resources[planet_id][resource] -= cnt;
		return cnt;
	}
	return free_resources[planet_id][resource];
}

int GameWrapper::getMyPlayerIdBySpecialty(Specialty specialty) const {
	for (int player_id = 0; player_id < game->players.size(); ++player_id) {
		if (isPlayerFriend(player_id) && getPlayerSpecialty(player_id) == specialty)
			return player_id;
	}
	return -1;
}

int GameWrapper::getEnemyPlayerIdBySpecialty(Specialty specialty) const {
	for (int player_id = 0; player_id < game->players.size(); ++player_id) {
		if (isPlayerEnemy(player_id) && getPlayerSpecialty(player_id) == specialty)
			return player_id;
	}
	return -1;
}

int GameWrapper::getOurBattlePower(int planet_id) const {
	return getMyTeamRobotCount(planet_id) +
		(getMyRobotCount(planet_id, Specialty::COMBAT) * game->combatUpgrade) / 100;
}
int GameWrapper::getEnemyBattlePower(int planet_id) const {
	return getEnemyTeamRobotCount(planet_id) +
		(getEnemyRobotCount(planet_id, Specialty::COMBAT) * game->combatUpgrade) / 100;
}

int GameWrapper::getPlanetFreeWorkerPlace(int planet_id) const {
	if (free_worker_place.find(planet_id) != free_worker_place.end())
		return free_worker_place.at(planet_id);
	return 0;
}
std::optional<Specialty> GameWrapper::getNextFreeSpecialty() const {
	std::unordered_set<Specialty> not_used = {Specialty::LOGISTICS, Specialty::COMBAT, Specialty::PRODUCTION};
	for (int player_id = 0; player_id < 6; ++player_id)
		if (isPlayerFriend(player_id)) {
			std::optional<Specialty> specialty = getPlayerSpecialty(player_id);
			if (specialty.has_value())
				not_used.erase(specialty.value());
		}
	if (not_used.empty())
		return std::nullopt;
	return *not_used.begin();
}
