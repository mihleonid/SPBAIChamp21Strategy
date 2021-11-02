#include "GameWrapper.h"
#include "graph.h"
#define GW GameWrapper
GW::GW(){
	inited=false;
}
void GW::update(const Game& g){
	game=&g;
	int N=g.planets.size();
	if(!inited){
		init();
		inited=1;
	}
	if(get_tick()<3){
		return; // See todo.txt for comments
	}
	ourScore=theirScore=0;
	for(int i=0;i<g.players.size();++i){
		availFly[i]=game->maxFlyingWorkerGroups;
		(is_player_evil(i)?theirScore:ourScore)+=g.players[i].score;
		assert(g.players[i].specialty);
		(is_player_evil(i)?whoCanEvil:whoCan)[g.players[i].specialty.value()]=i;
	}
	update_flyers();
	update_battles();
	update_moves();
}
void GW::update_flyers(){
	for(auto fwg:game->flyingWorkerGroups){
		int idx=fwg.playerIndex;
		assert(availFly[idx]);
		--availFly[idx];
		(is_player_evil(idx)?theirFlyingBots:ourFlyingBots)[get_specialty(idx)]+=fwg.number;
	}
}
void GW::update_battles(){
	for(int i=0;i<game->planets.size();++i){
		resources[i]=to_ll(game->planets[i].resources);
		BotSet total_we, total_they;
		for(auto wg:game->planets[i].workerGroups){
			(is_player_evil(wg.playerIndex)?total_they:total_we)[get_specialty(wg.playerIndex)]+=wg.number;
		}
		if(sum(total_they)){
			was_in_battle.insert(i);
		}
		tie(total_we, total_they)=battle(total_we, total_they);
	}
}
void GW::update_moves(){
	auto oldWait=waitingMoves;
	waitingMoves.clear();
	waitingMoves.resize(game->planets.size());
	for(int i=0;i<game->planets.size();++i){
		for(auto task:waitingMoves[i]){
			if(task.res){
				add_move(i, task.target, task.who);
			}
		}
	}
}
void GW::init(){
	availFly.resize(game->planets.size());
	ourBots.resize(game->planets.size());
	theirBots.resize(game->planets.size());
	resources.resize(game->planets.size());
	init_graph(*game);
}
//region Obvious function
int GW::get_tick(){
	return game->currentTick;
}
int& GW::get_avail_fly(Specialty spc){
	return get_avail_fly(who_can(spc));
}
int& GW::get_avail_fly(int spc){
	return availFly[spc];
}
int GW::get_my_index(){
	return game->myIndex;
}
int GW::get_our_team(){
	return get_team(get_my_index());
}
int GW::get_our_score(){
	return ourScore;
}
int GW::get_their_score(){
	return theirScore;
}
bool GW::is_player_evil(int idx){
	return get_our_team()-get_team(idx);
}
bool GW::is_player_good(int idx){
	return !is_player_evil(idx);
}
Specialty GW::get_specialty(int idx){
	assert((game->players[idx].specialty)&&"The specialty must exits. Use this function only if tick>=3");
	return game->players[idx].specialty.value();
}
Specialty GW::get_my_specialty(){
	return get_specialty(get_my_index());
}
int GW::who_can(Specialty s){
	return whoCan[s];
}
int GW::who_can_evil(Specialty s){
	return whoCanEvil[s];
}
int GW::get_all_our_bots(int planet){
	return sum(ourBots[planet]);
}
int GW::get_all_their_bots(int planet){
	return sum(theirBots[planet]);
}
int GW::get_team(int idx){
	return game->players[idx].teamIndex;
}
int GW::reserve_bot(int pl, int cnt, Specialty spc){
	int who=min(cnt, ourBots[pl][spc]);
	ourBots[pl][spc]-=who;
	return cnt-who;
}
void GW::reserve_res(int pl, int cnt, Resource r){
	assert(resources[pl][r]>=cnt);
	resources[pl][r]-=cnt;
}
//endregion
void GW::add_move(int from, int to, int who){
	add_move(from, to, who, get_tick());
}
void GW::add_move(int from, int to, int who, Resource r){
	add_move(from, to, who, r, get_tick());
}
void GW::add_move(int from, int to, int who, int when){
	if(!who){
		return;
	}
	if(who<0){
		warning();
	}
	targetting[to].first+=who;
	if(when<get_tick()){
		waitingMoves[from].push_back(MoveTask(when, to, who));
	}
}
void GW::add_move(int from, int to, int who, Resource r, int when){
}
vector<MoveAction> GW::get_moves(){
	return nowMoves;
}
int GW::our_might(int pl){
	return get_all_our_bots(pl)+(ourBots[pl][COMBAT]*game->combatUpgrade)/100;
}
int GW::their_might(int pl){
	return get_all_their_bots(pl)+(theirBots[pl][COMBAT]*game->combatUpgrade)/100;
}
pair<BotSet, BotSet> GW::battle(BotSet we, BotSet they){
	//Если что, то нас могут поколотить - лучше перебдеть и распределить всем нашим округление урона вверх.
	return {we, they}; // TODO Real battle so on one of BotSets will be 0,0,0
}
#undef GW
