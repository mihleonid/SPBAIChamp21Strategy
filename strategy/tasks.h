#pragma once
#include "common.h"
struct MoveTask{
	int start; // tick when from we can do this action. It is used to calculate targeting (who are flying to us) and prevent unwanted optimization (Про которую я ещё спрашивал, нужна ли она нам).
	int target;
	int who; // We do not use robots types, because we will automatically convert Task using any to task using fast robots
	optional<Resource> res;
	MoveTask();
	MoveTask(int start, int target, int who);
	MoveTask(int start, int target, int who, Resource);
	MoveTask(int start, int target, int who, optional<Resource>);
};
struct BuildTask{ // if we leave empty then it will be destroy task
	optional<BuildingType> type;
	BuildTask();
	BuildTask(BuildingType);
	BuildTask(optional<BuildingType>);
};
