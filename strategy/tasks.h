#pragma once
// #include "common.h"
#include "model/Specialty.hpp"
#include "model/Action.hpp"
#include "GameWrapper.h"
class Task {
public:
	virtual model::Specialty getActorSpecialty() const = 0;
	// Резервирует ресурсы и роботы под выполнение
	// Если хватает, то возвращает true
	// Иначе, возвращает true (и выполняется ли?)
	virtual bool reserve(GameWrapper&) = 0;
	virtual model::Action toAction() const = 0;
	virtual bool hasStopped(const GameWrapper&) const = 0;
};
/*struct MoveTask{
	int start; // tick when from we can do this action. It is used to calculate targeting (who are flying to us) and prevent unwanted optimization (Про которую я ещё спрашивал, нужна ли она нам).
	int target;
	int who; // We do not use robots types, because we will automatically convert Task using any to task using fast robots
	optional<Resource> res;
	MoveTask();
	MoveTask(int start, int target, int who);
	MoveTask(int start, int target, int who, Resource);
	MoveTask(int start, int target, int who, optional<Resource>);
};*/
