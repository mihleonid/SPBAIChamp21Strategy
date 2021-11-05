#include <set>
#include "graph.h"

Graph *Graph::instance = nullptr;

int Graph::lineDist(int from, int to) const { return dists_line[from][to]; }

void Graph::dijkstra(int from, const std::vector<std::vector<int>> &graph, std::vector<int> &dist, std::vector<std::vector<int>> &nexts) {
	std::vector<int> vertices_on_path(planets_count, 1e9); // number of vertices on path [from..i]
	std::set<std::pair<int, int>> open;             // set of pairs <distance, vertex>

	open.insert({0, from});
	vertices_on_path[from] = 0;
	dist[from] = 0;
	nexts[from][from] = from;

	while (!open.empty()) {
		int u = open.begin()->second;
		open.erase(open.begin());

		for (int p: graph[u]) {  // find ancestor with minimum vertices on path [from..p]
			if (dist[p] + lineDist(p, u) == dist[u] && vertices_on_path[p] + 1 < vertices_on_path[u]) {
				nexts[u][from] = p;
				vertices_on_path[u] = vertices_on_path[p] + 1;
			}
		}

		for (int v: graph[u]) {
			if (dist[u] + lineDist(u, v) < dist[v]) {
				open.erase({dist[v], v});
				dist[v] = dist[u] + lineDist(u, v);
				open.insert({dist[v], v});
			}
		}
	}
}

std::vector<std::vector<int>> Graph::build_graph(int max_dist) {
	std::vector<std::vector<int>> graph(planets_count);
	for (int a = 0; a < planets_count; ++a) {
		for (int b = 0; b < planets_count; ++b) {
			if (lineDist(a, b) <= max_dist && a != b) { // we don`t need loops
				graph[a].push_back(b);
			}
		}
	}
	return graph;
}

int Graph::nextBySpecialty(int from, int to, model::Specialty specialty) {
	if (specialty == model::Specialty::LOGISTICS) {
		return next_logistic_specialty[from][to];
	} else {
		return next_any_specialty[from][to];
	}
}

int Graph::distBySpecialty(int from, int to, model::Specialty specialty) {
	if (specialty == model::Specialty::LOGISTICS) {
		return dists_logistic_specialty[from][to];
	} else {
		return dists_any_specialty[from][to];
	}
}

Graph::Graph(const model::Game &game) {
	planets_count = game.planets.size();
	dists_line.resize(planets_count, std::vector<int>(planets_count));
	for (int a = 0; a < planets_count; ++a) {
		for (int b = 0; b < planets_count; ++b) {
			dists_line[a][b] = std::abs(game.planets[a].x - game.planets[b].x) +
				std::abs(game.planets[a].y - game.planets[b].y);
		}
	}

	dists_any_specialty.resize(planets_count, std::vector<int>(planets_count, 1e9));
	next_any_specialty.resize(planets_count, std::vector<int>(planets_count, -1));

	dists_logistic_specialty.resize(planets_count, std::vector<int>(planets_count, 1e9));
	next_logistic_specialty.resize(planets_count, std::vector<int>(planets_count, -1));

	std::vector<std::vector<int>> graph_any_speciality = build_graph(game.maxTravelDistance);
	std::vector<std::vector<int>> graph_logistic_speciality = build_graph(game.maxTravelDistance + game.logisticsUpgrade);

	for (int planet_id = 0; planet_id < planets_count; ++planet_id) {
		dijkstra(planet_id, graph_any_speciality, dists_any_specialty[planet_id], next_any_specialty);
		dijkstra(planet_id, graph_logistic_speciality, dists_logistic_specialty[planet_id], next_logistic_specialty);
	}
}
