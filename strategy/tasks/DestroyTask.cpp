#include "DestroyTask.h"

#include <vector>

DestroyTask::DestroyTask(int planet_id, int robot_cnt, Specialty sp) : planet_id(planet_id), robot_cnt(robot_cnt), specialty(sp) {}

bool DestroyTask::reserve(GameWrapper &game_wrapper) {
	// auto specialty =
	return game_wrapper.reserveRobots(planet_id, game_wrapper.getMyPlayerIdBySpecialty(getActorSpecialty()), robot_cnt) == robot_cnt;
}

model::Action DestroyTask::toAction() const {
	return model::Action(std::vector<MoveAction>(), {BuildingAction(planet_id, std::nullopt)}, std::nullopt);
}

bool DestroyTask::hasStopped(const GameWrapper &game_wrapper) const {
	return !game_wrapper.getBuilding(planet_id).has_value();
}


