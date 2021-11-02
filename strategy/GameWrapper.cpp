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
	for(const auto& player:g.players){
		(player.teamIndex-get_our_team()?theirScore:ourScore)+=player.score;
	}
}
void GW::init(){
	init_graph(*game);
}
int GW::get_tick(){
	return game->currentTick;
}
int GW::get_avail_fly(Specialty spc){
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
#undef GW
