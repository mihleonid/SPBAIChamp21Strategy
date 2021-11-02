#include "common.h"
int currentRandom=0;
int mrand(){
	return currentRandom=(currentRandom*7+5)%INF;
}
