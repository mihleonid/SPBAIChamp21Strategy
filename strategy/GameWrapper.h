#include "common.h"

class GameWrapper{
	private:
		const Game* game;
		bool inited;
		void init();
		vi availFly;
		int ourScore, theirScore;
		dict<Specialty, int> whoCan;
		dict<Specialty, int> whoCanEvil;
		BotSet ourFlyingBots;
		BotSet theirFlyingBots;
		vector<BotSet> ourBots;
		vector<BotSet> theirBots;
		vector<ResSet> resources;
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
		Specialty get_specialty(int idx); // Returns specialty of playert with index == idx;
		// reserve_*_bot works like this: taking bots from planet and return them to you. If there is not enough bots then take some of them. Priority is for fast: FAST, WARRIORS, WORKERS. For work: WORKERS, WARRIORS, FAST. TODO rewrite reservation: if we want to move and there are not availFly, reserve anather and so on.
		BotSet reserve_fast_bot(int);
		BotSet reserve_work_bot(int);
		void reserve_res(Resource, int);
		int get_team(int);// Returns team by player index
		pair<BotSet, BotSet> battle(BotSet a, BotSet b); // Кто выживет.
};
