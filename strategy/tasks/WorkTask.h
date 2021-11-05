#pragma once
#include "Task.h"

class WorkTask : public Task {
public:
	WorkTask(int planet_id, int robot_cnt, Specialty);

	inline model::Specialty getActorSpecialty() const override { return specialty; }
	// Резервирует ресурсы и роботы под выполнение
	// Если хватает, то возвращает true
	// Иначе, возвращает false и не выполняется
	inline bool reserve(GameWrapper& game_wrapper) override { return game_wrapper.reserveMyRobots(planet_id, specialty, robot_cnt); }
	inline model::Action toAction() const override { return Action(); }
	// Работа только один тик
	inline bool hasStopped(const GameWrapper&) const override { return true; }
private:
	int planet_id, robot_cnt;
	Specialty specialty;
};
