#include "MoveRobotsTask.h"

#include <algorithm>
#include "../graph.h"

MoveRobotsTask::MoveRobotsTask(int planet_from, int planet_to, int robot_cnt, Specialty specialty) :
	planet_to(planet_to),
	robot_cnt(robot_cnt), specialty(specialty), current_planet(planet_from) {
	next_launch_timer = 0;
	next_arrival_planet = planet_from;
}

bool MoveRobotsTask::reserve(GameWrapper &game_wrapper) {
	if (next_launch_timer == 0) {
		current_planet = next_arrival_planet;
		if (game_wrapper.getEnemyBattlePower(current_planet)) {
			return false;
		}
		if (current_planet == planet_to) {
			return false;
		}
		robot_cnt = std::min(game_wrapper.getMyFreeRobotCount(current_planet, specialty), robot_cnt);
		game_wrapper.reserveMyRobots(current_planet, specialty, robot_cnt);
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

model::Action MoveRobotsTask::toAction() const {
	if (will_launch_this_tick)
		return Action({MoveAction(current_planet, next_arrival_planet, robot_cnt, std::nullopt)},
					  std::vector<BuildingAction>(), std::nullopt);
	else
		return Action();
}

bool MoveRobotsTask::hasStopped(const GameWrapper &game_wrapper) const {
	return (current_planet == planet_to && next_launch_timer == 0) || robot_cnt <= 0;
}
