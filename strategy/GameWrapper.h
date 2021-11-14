#pragma once
#include <vector>
#include <unordered_map>

#include "model/Game.hpp"
#include "model/Specialty.hpp"

using namespace model;

class GameWrapper {
public:
	void update(const Game &);

	inline const std::vector<Planet> & getPlanets() const { return game->planets; }
	inline const std::vector<FlyingWorkerGroup>& getFlyingWorkerGroups() const { return game->flyingWorkerGroups; }
	inline int getCombatUpgrade() const { return game->combatUpgrade; }

	int getRobotCount(int planet_id, int player_id) const;
	int getFreeRobotCount(int planet_id, int player_id) const;

	inline int getMyRobotCount(int planet_id, Specialty specialty) const { return getRobotCount(planet_id, getMyPlayerIdBySpecialty(specialty)); }
	inline int getMyFreeRobotCount(int planet_id, Specialty specialty) const { return getFreeRobotCount(planet_id, getMyPlayerIdBySpecialty(specialty)); }

	inline int getEnemyRobotCount(int planet_id, Specialty specialty) const { return getRobotCount(planet_id, getEnemyPlayerIdBySpecialty(specialty)); }
	inline int getEnemyFreeRobotCount(int planet_id, Specialty specialty) const { return getFreeRobotCount(planet_id, getEnemyPlayerIdBySpecialty(specialty)); }

	int getMyTeamRobotCount(int planet_id) const;
	int getMyTeamFreeRobotCount(int planet_id) const;

	int getEnemyTeamRobotCount(int planet_id) const;
	int getEnemyTeamFreeRobotCount(int planet_id) const;

	int getMyTotalRobotCount() const;
	int getMyTotalFreeRobotCount() const;

	int getEnemyTotalRobotCount() const;
	int getEnemyTotalFreeRobotCount() const;

	int getMyTeamScore() const;
	int getEnemyTeamScore() const;

	bool isPlayerFriend(int player_id) const; // Our team's player
	bool isPlayerEnemy(int player_id) const;  // Their team's player

	std::unordered_map<Resource, int> getResourcesCount(int planet_id) const;
	int getResourceCount(int planet_id, Resource resource) const;

	inline int getMyTeamId() const { return getPlayerTeamId(getMyPlayerId()); }
	int getEnemyTeamId() const;

	// Если не хватает, то не резервирует, а возвращает, сколько там есть
	// Иначе возвращает сколько зарезервировало
	inline int reserveMyRobots(int planet_id, Specialty sp, int cnt) { return reserveRobots(planet_id, getMyPlayerIdBySpecialty(sp), cnt); }
	inline int reserveEnemyRobots(int planet_id, Specialty sp, int cnt) { return reserveRobots(planet_id, getEnemyPlayerIdBySpecialty(sp), cnt); }
	int reserveRobots(int planet_id, int player_id, int cnt);
	int reserveResources(int planetId, Resource resource, int cnt);

	inline std::optional<Building> getBuilding(int planet_id) const { return game->planets[planet_id].building; }
	inline BuildingProperties getBuildingProperties(BuildingType t) const { return game->buildingProperties.at(t); }

	bool isEnoughResourcesToBuild(int planet_id, BuildingType) const;

	inline int getCurrentTick() const { return game->currentTick; }

	inline int getPlayerTeamId(int player_id) const { return game->players[player_id].teamIndex; }
	inline int getMyPlayerId() const { return game->myIndex; }

	inline std::optional<Specialty> getPlayerSpecialty(int player_id) const { return game->players[player_id].specialty; }

	inline std::optional<Specialty> getMySpecialty() const { return getPlayerSpecialty(getMyPlayerId()); }

	// Возвращает индекс игрока в нашей команде с данной специализацией
	// Если такого не находит, то возвращает -1
	int getMyPlayerIdBySpecialty(Specialty) const;
	// Возвращает индекс игрока во вражеской команде с данной специализацией
	// Если такого не находит, то возвращает -1
	int getEnemyPlayerIdBySpecialty(Specialty) const;

	int getOurBattlePower(int planet) const; // Return our battle power.
	int getEnemyBattlePower(int planet) const; // Return their battle power.

	inline int getPlayerAvailableFlyingGroups(int player_id) const { return available_flying_groups.at(player_id); }
	inline void addPlayerFlyingGroup(int player_id) { available_flying_groups[player_id] -= 1; }

	int getPlanetFreeWorkerPlace(int planet_id) const;
	inline void reservePlanetFreeWorkerPlace(int planet_id, int cnt) { free_worker_place[planet_id] -= cnt; }

	inline int getPlayerStartingPlanet(int player_id) const { return player_starting_planet.at(player_id); }

	// inline Specialty getPlannedSpeciality(int player_id) const { return my_specialities.at(player_id); }
	std::optional<Specialty> getNextFreeSpecialty() const;
private:
	const Game* game;
	std::unordered_map<int, int> player_starting_planet;
	std::vector<std::unordered_map<int, int>> free_robots;         //index is a planet's id
	std::vector<std::unordered_map<Resource, int>> free_resources;
	std::unordered_map<int, int> available_flying_groups;
	std::unordered_map<int, int> free_worker_place; // planet_id -> number of free worker place

	// std::unordered_map<int, Specialty> my_specialities; // player_id -> Speciality
};
