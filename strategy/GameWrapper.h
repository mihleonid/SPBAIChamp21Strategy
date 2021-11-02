#include "common.h"
#include "tasks.h"

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
		vector<BotSet> ourBots; // Bots on planets, battle and done actions excluded.
		vector<BotSet> theirBots;
		vector<ResSet> resources; // Resources on planets, used excluded
		vector<MoveTask> waitingMoves;
		vector<MoveTask> nowMoves;

		int get_team(int);// Returns team by player index
		int get_our_team();
		int get_my_index();
		int get_avail_fly(int idx);
		bool is_player_good(int idx); // Our team's player
		bool is_player_evil(int idx); // Their team's player
		int who_can(Specialty); // Returns who from our team has such specialty
		int who_can_evil(Specialty); // Returns who from their team has such specialty
		Specialty get_specialty(int idx); // Returns specialty of playert with index == idx;
	public:
		GameWrapper();
		void update(const Game&);

		int get_tick();
		int get_avail_fly(Specialty); // Return max availible flying worker group cnt
		Specialty get_my_specialty();
		int get_our_score();
		int get_their_score();

		int reserve_bot(int planet, int cnt, Specialty); // Reserves bots and returnes rest that was not reserved
		void reserve_res(int planet, int cnt,  Resource); // Reserves resources or crash
		int get_all_our_bots(int planet); // Return sum of ourBots

		void add_move(int from, int to, int who);
		void add_move(int from, int to, int who, int when);
		void add_move(int from, int to, int who, Resource);
		void add_move(int from, int to, int who, int when, Resource);

		int our_might(int planet); // Return our battle power.
		int their_might(int planet); // Return their battle power.
		pair<BotSet, BotSet> battle(BotSet a, BotSet b); // Кто выживет.
};
