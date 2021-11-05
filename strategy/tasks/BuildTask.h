#pragma once
#include "Task.h"

class BuildTask : public Task {
public:
	BuildTask(int planet_id, BuildingType, int robot_cnt, Specialty);

	inline model::Specialty getActorSpecialty() const override { return specialty; }
	// Резервирует ресурсы и роботы под выполнение
	// Если хватает, то возвращает true
	// Иначе, возвращает false и не выполняется
	bool reserve(GameWrapper& game_wrapper) override;
	// Спамит строительство
	model::Action toAction() const override;
	// Останавливается, когда на планете есть какое-либо здание
	bool hasStopped(const GameWrapper&) const override;

private:
	int planet_id, robot_cnt;
	BuildingType type;
	Specialty specialty;

	bool will_execute_this_tick = true;
};
