#pragma once
#include <functional>
#include "Task.h"

class MoveResourceTask : public Task {
public:
	MoveResourceTask(int planet_from, int planet_to, Resource, int resource_cnt, Specialty);

	inline model::Specialty getActorSpecialty() const override { return specialty; }

	bool reserve(GameWrapper& game_wrapper) override;

	model::Action toAction() const override;

	bool hasStopped(const GameWrapper&) const override;

	inline int getArrivalPlanet() const { return planet_to; }
	inline int getPacketCount() const { return resource_cnt; }
	inline int getPacketLoss() const { return departure_cnt - getPacketCount(); }
	inline Resource getResource() const { return resource; }

	inline void setOnDone(const std::function<void(const MoveResourceTask&)>& f) { on_done = f; }

	inline ~MoveResourceTask() {
		on_done(*this);
	}

private:
	int current_planet, next_arrival_planet, planet_to, resource_cnt, next_launch_timer;
	Resource resource;
	const int departure_cnt;
	Specialty specialty;
	bool will_launch_this_tick = true;

	std::function<void(const MoveResourceTask&)> on_done = [](const MoveResourceTask&){};
};
