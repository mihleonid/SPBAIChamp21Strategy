#include "GameWrapper.h"
#include "graph.h"
#define GW GameWrapper
GW::GW(){
	inited=false;
}
void GW::update(const Game& g){
	game=&g;
	if(!inited){
		init();
		inited=1;
	}
	ourScore=theirScore=0;
	for(int i=0;i<g.players.size();++i){
		(g.players[i].teamIndex-get_our_team()?theirScore:ourScore)+=g.players[i].score;
		if(g.players[i].specialty){
			(g.players[i].teamIndex-get_our_team()?whoCanEvil:whoCan)[g.players[i].specialty.value()]=i;
		}
	}
	if(g.players[get_my_index()].specialty){
		mySpecialty=g.players[get_my_index()].specialty.value();
	}
}
void GW::init(){
	init_graph(*game);
}
int GW::get_tick(){
	return game->currentTick;
}
int GW::get_avail_fly(Specialty spc){
	return get_avail_fly(who_can(spc));
}
int GW::get_avail_fly(int spc){
	return availFly[spc];
}
int GW::get_my_index(){
	return game->myIndex;
}
int GW::get_our_team(){
	return game->players[get_my_index()].teamIndex;
}
int GW::get_our_score(){
	return ourScore;
}
int GW::get_their_score(){
	return theirScore;
}
bool GW::is_player_evil(int idx){
	return get_our_team()-game->players[idx].teamIndex;
}
bool GW::is_player_good(int idx){
	return !is_player_evil(idx);
}
Specialty GW::get_my_specialty(){
	return mySpecialty;
}
int GW::who_can(Specialty s){
	return whoCan[s];
}
int GW::who_can_evil(Specialty s){
	return whoCanEvil[s];
}
#undef GW
