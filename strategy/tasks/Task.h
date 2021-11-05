#pragma once
// #include "common.h"
#include "model/Specialty.hpp"
#include "model/Action.hpp"
#include "strategy/GameWrapper.h"
class Task {
public:
	virtual model::Specialty getActorSpecialty() const = 0;
	// Резервирует ресурсы и роботы под выполнение
	// Если хватает, то возвращает true
	// Иначе, возвращает false и не выполняется
	virtual bool reserve(GameWrapper&) = 0;
	virtual model::Action toAction() const = 0;
	virtual bool hasStopped(const GameWrapper&) const = 0;

	virtual ~Task() = default;
};
