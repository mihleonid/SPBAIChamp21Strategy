// #include "common.h"
#pragma once
#include "model/Game.hpp"
namespace graph{
	int line_dist(int from, int to); // Length of line (x+y)
	int any_dist(int from, int to); // Length of path for any specialization
	int any_next(int from, int to); // Number of next planet for any specialization
	int fast_dist(int from, int to); // Length of path for logistics specialization
	int fast_next(int from, int to); // Number of next planet for logistics specialization
	void init_graph(const model::Game& g); // Call it just once in the begining of game
}
using graph::line_dist;
using graph::any_dist;
using graph::fast_dist;
using graph::any_next;
using graph::fast_next;
using graph::init_graph;
