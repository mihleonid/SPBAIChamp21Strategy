#pragma once
#include "Task.h"

class DestroyTask : public Task {
public:
	DestroyTask(int planet_id, int robot_cnt, Specialty);

	inline model::Specialty getActorSpecialty() const { return specialty; }
	// Резервирует ресурсы и роботы под выполнение
	// Если хватает, то возвращает true
	// Иначе, возвращает false и не выполняется
	bool reserve(GameWrapper& game_wrapper) override;
	model::Action toAction() const override;
	bool hasStopped(const GameWrapper&) const override;
private:
	int planet_id, robot_cnt;
	Specialty specialty;
};
