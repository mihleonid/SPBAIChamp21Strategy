#include "MoveResourceTask.h"

#include <algorithm>
#include "../graph.h"

MoveResourceTask::MoveResourceTask(int planet_from, int planet_to, Resource resource, int resource_cnt, Specialty specialty) :
	planet_to(planet_to), resource(resource), departure_cnt(resource_cnt),
	resource_cnt(resource_cnt), specialty(specialty), current_planet(planet_from) {
	next_launch_timer = 0;
	next_arrival_planet = planet_from;
}

bool MoveResourceTask::reserve(GameWrapper &game_wrapper) {
	if (next_launch_timer == 0) {
		current_planet = next_arrival_planet;
		if (current_planet == planet_to) {
			return false;
		}
		int robot_cnt = game_wrapper.getMyFreeRobotCount(current_planet, specialty);
		int resources = game_wrapper.getResourceCount(current_planet, resource);

		resource_cnt = std::min({robot_cnt, resources, resource_cnt});
		game_wrapper.reserveResources(current_planet, resource, resource_cnt);
		game_wrapper.reserveMyRobots(current_planet, specialty, resource_cnt);
		if (game_wrapper.getPlayerAvailableFlyingGroups(game_wrapper.getMyPlayerIdBySpecialty(specialty)) > 0 &&
			game_wrapper.getEnemyBattlePower(current_planet) == 0) {
			game_wrapper.addPlayerFlyingGroup(game_wrapper.getMyPlayerIdBySpecialty(specialty));

			next_arrival_planet = Graph::getInstance()->nextBySpecialty(current_planet, planet_to, specialty);
			next_launch_timer = Graph::getInstance()->distBySpecialty(current_planet, next_arrival_planet, specialty) - 1;
			will_launch_this_tick = true;
		}
	} else {
		next_launch_timer--;
		will_launch_this_tick = false;
	}
	return will_launch_this_tick;
}

model::Action MoveResourceTask::toAction() const {
	if (will_launch_this_tick)
		return Action({MoveAction(current_planet, next_arrival_planet, resource_cnt, resource)},
					  std::vector<BuildingAction>(), std::nullopt);
	else
		return {};
}

bool MoveResourceTask::hasStopped(const GameWrapper &game_wrapper) const {
	return (current_planet == planet_to && next_launch_timer == 0) || resource_cnt <= 0;
}
