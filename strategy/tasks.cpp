#include "tasks.h"
MoveTask::MoveTask(){}
MoveTask::MoveTask(int st, int tar, int wh):start(st), target(tar), who(wh){}
MoveTask::MoveTask(int st, int tar, int wh, Resource r):start(st), target(tar), who(wh), res(r){}
MoveTask::MoveTask(int st, int tar, int wh, optional<Resource> r):start(st), target(tar), who(wh), res(r){}
BuildTask::BuildTask(){}
BuildTask::BuildTask(BuildingType t):type(t){}
BuildTask::BuildTask(optional<BuildingType> t):type(t){}
