#include "GameWrapper.h"

#include <algorithm>

void GameWrapper::update(const Game &g) {
	game = g;

	free_robots.resize(game.planets.size());
	free_resources.resize(game.planets.size());

	for (int planet_id = 0; planet_id < game.planets.size(); ++planet_id) {
		free_robots[planet_id].clear();
		free_resources[planet_id].clear();

		for (const WorkerGroup& worker_group : game.planets[planet_id].workerGroups) {
			free_robots[planet_id][worker_group.playerIndex] += worker_group.number;
		}

		for (const auto& [resource, amount] : game.planets[planet_id].resources) {
			free_resources[planet_id][resource] += amount;
		}
	}

	for (int player_id = 0; player_id < game.players.size(); ++player_id) {
		int current_flying_groups =
			std::count_if(game.flyingWorkerGroups.begin(),
						  game.flyingWorkerGroups.end(),
						  [player_id](const FlyingWorkerGroup &group) { return group.playerIndex == player_id; });
		available_flying_groups[player_id] = game.maxFlyingWorkerGroups - current_flying_groups;
	}
}

int GameWrapper::getRobotCount(int planet_id, int player_id) const {
	int robots = 0;
	for (const WorkerGroup &worker_group: game.planets[planet_id].workerGroups) {
		if (worker_group.playerIndex == player_id) {
			robots += worker_group.number;
		}
	}
	return robots;
}

int GameWrapper::getMyTeamRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < 3; ++i) {
		res += getMyRobotCount(planet_id, (Specialty) i);
	}
	return res;
}

int GameWrapper::getMyTeamFreeRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < 3; ++i) {
		res += getMyFreeRobotCount(planet_id, (Specialty) i);
	}
	return res;
}

int GameWrapper::getEnemyTeamRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < 3; ++i)
		res += getEnemyRobotCount(planet_id, (Specialty) i);
	return res;
}

int GameWrapper::getEnemyTeamFreeRobotCount(int planet_id) const {
	int res = 0;
	for (int i = 0; i < 3; ++i) {
		res += getEnemyFreeRobotCount(planet_id, (Specialty) i);
	}
	return res;
}

int GameWrapper::getMyTotalRobotCount() const {
	int res = 0;
	for (int i = 0; i < game.planets.size(); ++i)
		res += getMyTeamRobotCount(i);
	return res;
}
int GameWrapper::getMyTotalFreeRobotCount() const {
	int res = 0;
	for (int i = 0; i < game.planets.size(); ++i)
		res += getMyTeamFreeRobotCount(i);
	return res;
}

int GameWrapper::getEnemyTotalRobotCount() const {
	int res = 0;
	for (int i = 0; i < game.planets.size(); ++i)
		res += getEnemyTeamRobotCount(i);
	return res;
}
int GameWrapper::getEnemyTotalFreeRobotCount() const {
	int res = 0;
	for (int i = 0; i < game.planets.size(); ++i)
		res += getEnemyTeamFreeRobotCount(i);
	return res;
}

int GameWrapper::getMyTeamScore() const {
	int res = 0;
	for (int i = 0; i < game.players.size(); ++i) {
		if (getPlayerTeamId(i) == getMyTeamId()) {
			res += game.players[i].score;
		}
	}
	return res;
}
int GameWrapper::getEnemyTeamScore() const {
	int result = 0;
	for (int player_id = 0; player_id < game.players.size(); ++player_id) {
		if (isPlayerEnemy(player_id)) {
			result += game.players[player_id].score;
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
	return game.planets[planet_id].resources;
}

int GameWrapper::getEnemyTeamId() const {
	for (int player_id = 0; player_id < game.players.size(); ++player_id) {
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
	for (int player_id = 0; player_id < game.players.size(); ++player_id) {
		if (isPlayerFriend(player_id) && getPlayerSpecialty(player_id) == specialty)
			return player_id;
	}
	return -1;
}

int GameWrapper::getEnemyPlayerIdBySpecialty(Specialty specialty) const {
	for (int player_id = 0; player_id < game.players.size(); ++player_id) {
		if (isPlayerEnemy(player_id) && getPlayerSpecialty(player_id) == specialty)
			return player_id;
	}
	return -1;
}

int GameWrapper::getOurBattlePower(int planet_id) const {
	return getMyTeamRobotCount(planet_id) +
		(getMyRobotCount(planet_id, Specialty::COMBAT) * game.combatUpgrade) / 100;
}
int GameWrapper::getEnemyBattlePower(int planet_id) const {
	return getEnemyTeamRobotCount(planet_id) +
		(getEnemyRobotCount(planet_id, Specialty::COMBAT) * game.combatUpgrade) / 100;
}

int GameWrapper::getPlanetFreeWorkerPlace(int planet_id) const {
	std::optional<Building> building = getBuilding(planet_id);
	if (building.has_value()) {
		BuildingProperties info = getBuildingProperties(building.value().buildingType);
		return info.maxWorkers - getMyTeamRobotCount(planet_id);
	}
	return 0;
}
