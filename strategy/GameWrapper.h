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
		vector<vector<MoveTask>> waitingMoves; //TODO discuss may be better not to have queue of moves, but dict[target -> task];
		vector<MoveAction> nowMoves;

		vector<pair<int, ResSet>> targetting; // Who is comming to us
		set<int> was_in_battle; // Indexes of planets

		int get_team(int);// Returns team by player index
		int get_our_team();
		int get_my_index();
		int& get_avail_fly(int idx);
		int& get_avail_fly(Specialty); // Return max availible flying worker group cnt
		bool is_player_good(int idx); // Our team's player
		bool is_player_evil(int idx); // Their team's player
		int who_can(Specialty); // Returns who from our team has such specialty
		int who_can_evil(Specialty); // Returns who from their team has such specialty
		Specialty get_specialty(int idx); // Returns specialty of playert with index == idx;

		void add_move(int from, int to, int who, int when);
		void add_move(int from, int to, int who, Resource, int when);
		void add_move(int from, MoveTask);

		void update_flyers();
		void update_battles();
		void update_moves();

	public:
		GameWrapper();
		void update(const Game&);
		vector<MoveAction> get_moves();
		vector<BuildingAction> get_builds();

		int get_tick();
		Specialty get_my_specialty();
		int get_our_score();
		int get_their_score();

		int reserve_bot(int planet, int cnt, Specialty); // Reserves bots and returnes rest that was not reserved
		void reserve_res(int planet, int cnt,  Resource); // Reserves resources or crash
		int get_all_our_bots(int planet); // Return sum of ourBots on planet
		int get_all_their_bots(int planet);

		void add_move(int from, int to, int who);
		void add_move(int from, int to, int who, Resource);

		int our_might(int planet); // Return our battle power.
		int their_might(int planet); // Return their battle power.
		pair<BotSet, BotSet> battle(BotSet a, BotSet b); // Кто выживет.
};
