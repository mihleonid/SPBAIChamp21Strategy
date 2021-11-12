#include <iostream>
#include "BuildTask.h"

BuildTask::BuildTask(int planet_id, BuildingType t, int robot_cnt, Specialty sp) :
	planet_id(planet_id), type(t), robot_cnt(robot_cnt), specialty(sp) {}

bool BuildTask::reserve(GameWrapper &game_wrapper) {
	will_execute_this_tick = true;
	// std::cerr << "BUILDTASK Reserved: " << game_wrapper.getMyTeamFreeRobotCount(planet_id) << std::endl;
	std::optional<model::Building> building = game_wrapper.getBuilding(planet_id);

	if (!building.has_value()) {
		for (auto& [resource, amount] : game_wrapper.getBuildingProperties(type).buildResources) {
			int cnt = game_wrapper.reserveResources(planet_id, resource, amount);
			if (cnt < amount) {
				will_execute_this_tick = false;
				game_wrapper.reserveResources(planet_id, resource, cnt);
			}
		}
	}

	int reserved_cnt = game_wrapper.reserveRobots(planet_id,game_wrapper.getMyPlayerIdBySpecialty(getActorSpecialty()), robot_cnt);
	if (reserved_cnt < robot_cnt) {
		will_execute_this_tick = false;
		game_wrapper.reserveRobots(planet_id, game_wrapper.getMyPlayerIdBySpecialty(getActorSpecialty()), reserved_cnt);
	}
	return will_execute_this_tick;
}

model::Action BuildTask::toAction() const {
	// if (will_execute_this_tick)
		return model::Action(std::vector<MoveAction>(), {BuildingAction(planet_id, type)}, std::nullopt);
	// else
		// return model::Action();
}

bool BuildTask::hasStopped(const GameWrapper &game_wrapper) const {
	std::optional<model::Building> building = game_wrapper.getBuilding(planet_id);
	return building.has_value() && building.value().health == game_wrapper.getBuildingProperties(building.value().buildingType).maxHealth;
}
