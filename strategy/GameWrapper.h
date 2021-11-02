#include "common.h"

class GameWrapper{
	private:
		const Game* game;
		bool inited;
		void init();
		vi availFly;
		int ourScore, theirScore;
		Specialty mySpecialty;
		dict<Specialty, int> whoCan;
		dict<Specialty, int> whoCanEvil;
	public:
		GameWrapper();
		void update(const Game&);
		int get_tick();
		int get_avail_fly(Specialty); // Return max availible flying worker group cnt
		int get_avail_fly(int idx);
		int get_my_index();
		Specialty get_my_specialty();
		int get_our_team();
		int get_our_score();
		int get_their_score();
		bool is_player_good(int idx); // Our team's player
		bool is_player_evil(int idx); // Their team's player
		int who_can(Specialty); // Returns who from our team has such specialty
		int who_can_evil(Specialty); // Returns who from their team has such specialty
};
