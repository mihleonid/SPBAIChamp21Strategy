#include "common.h"

class GameWrapper{
	private:
		const Game* game;
		bool inited;
		void init();
		dict<Specialty, int> availFly;
	public:
		GameWrapper();
		void update(const Game&);
		int get_tick();
		int get_avail_fly(Specialty); // Return max availible flying worker group cnt
		int get_my_index();
		Specialty get_my_specialty();
		int get_our_team();
};
