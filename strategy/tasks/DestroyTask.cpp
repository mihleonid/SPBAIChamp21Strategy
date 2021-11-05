#include "DestroyTask.h"

#include <vector>

DestroyTask::DestroyTask(int planet_id, int robot_cnt, Specialty sp) : planet_id(planet_id), robot_cnt(robot_cnt), specialty(sp) {}

bool DestroyTask::reserve(GameWrapper &game_wrapper) {
	will_execute_this_tick = true;
	int reserved_cnt = game_wrapper.reserveRobots(planet_id, game_wrapper.getMyPlayerIdBySpecialty(getActorSpecialty()), robot_cnt);
	if (reserved_cnt < robot_cnt) {
		will_execute_this_tick = false;
		game_wrapper.reserveRobots(planet_id, game_wrapper.getMyPlayerIdBySpecialty(getActorSpecialty()), reserved_cnt);
	}
	return will_execute_this_tick;
}

model::Action DestroyTask::toAction() const {
	if (will_execute_this_tick)
		return model::Action(std::vector<MoveAction>(), {BuildingAction(planet_id, std::nullopt)}, std::nullopt);
	else
		return model::Action();
}

bool DestroyTask::hasStopped(const GameWrapper &game_wrapper) const {
	return !game_wrapper.getBuilding(planet_id).has_value();
}


