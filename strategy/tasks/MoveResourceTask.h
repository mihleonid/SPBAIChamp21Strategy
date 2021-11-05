#pragma once
#include "Task.h"

class MoveResourceTask : public Task {
public:
	MoveResourceTask(int planet_from, int planet_to, Resource, int resource_cnt, Specialty);

	inline model::Specialty getActorSpecialty() const override { return specialty; }

	bool reserve(GameWrapper& game_wrapper) override;

	model::Action toAction() const override;

	bool hasStopped(const GameWrapper&) const override;

private:
	int current_planet, next_arrival_planet, planet_to, resource_cnt, next_launch_timer;
	Resource resource;
	Specialty specialty;
	bool will_launch_this_tick = true;
};
