#pragma once
#include "model/Game.hpp"
#include <vector>

class Graph {
public:
	int lineDist(int from, int to) const;       // Length of straight line (x+y)

	int distBySpecialty(int from, int to, model::Specialty);
	int nextBySpecialty(int from, int to, model::Specialty);

	inline static Graph* getInstance() { return instance; }
	inline static void init(const model::Game& game) { instance = new Graph(game); }
	inline static void cleanup() { delete instance; }
private:
	static Graph* instance;
	Graph(const model::Game&);
	int planets_count;
	std::vector<std::vector<int>> dists_any_specialty;
	std::vector<std::vector<int>> next_any_specialty;
	std::vector<std::vector<int>> dists_logistic_specialty;
	std::vector<std::vector<int>> next_logistic_specialty;
	std::vector<std::vector<int>> dists_line;

	void dijkstra(int from, const std::vector<std::vector<int>> &graph, std::vector<int> &dist, std::vector<std::vector<int>> &nexts);
	std::vector<std::vector<int>> build_graph(int max_dist);
};
