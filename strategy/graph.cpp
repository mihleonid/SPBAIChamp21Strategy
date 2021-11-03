#include "graph.h"

namespace graph {
	int N;
	vvi dists_any;
	vvi nexts_any;
	vvi dists_fast;
	vvi nexts_fast;
	vvi dists_line;

	int line_dist(int a, int b) { return dists_line[a][b]; }

	int any_dist(int fr, int to) { return dists_any[fr][to]; }

	int fast_dist(int fr, int to) { return dists_fast[fr][to]; }

	int any_next(int fr, int to) { return nexts_any[fr][to]; }

	int fast_next(int fr, int to) { return nexts_fast[fr][to]; }

	auto calc_neib(int m_dist) {
		vvi ans(N);
		for (int a = 0; a < N; ++a) {
			for (int b = 0; b < N; ++b) {
				if (a == b) {
					continue;
				}
				if (line_dist(a, b) <= m_dist) {
					ans[a].push_back(b);
				}
			}
		}
		return ans;
	}

	void dijkstra(int from, const vvi &neib, vvi &d, vvi &nexts) {
		vector<int> vertices_on_path(N, INF); // number of vertices on path [from..i]
		set<pair<int, int>> open;             // set of pairs <distance, vertex>

		open.insert({0, from});
		vertices_on_path[from] = 0;
		d[from][from] = 0;
		nexts[from][from] = from;

		while (!open.empty()) {
			int u = open.begin()->second;
			open.erase(open.begin());

			for (int p : neib[u]) {  // find ancestor with minimum vertices on path [from..p]
				if (d[from][p] + line_dist(p, u) == d[from][u] && vertices_on_path[p] + 1 < vertices_on_path[u]) {
					nexts[u][from] = p;
					vertices_on_path[u] = vertices_on_path[p] + 1;
				}
			}

			for (int v: neib[u]) {
				if (d[from][u] + line_dist(u, v) < d[from][v]) {
					open.erase({d[from][v], v});
					d[from][v] = d[from][u] + line_dist(u, v);
					open.insert({d[from][v], v});
				}
			}
		}
	}

	auto calc_all(int m_dist) {
		auto neib = calc_neib(m_dist);
		vvi dists(N, vi(N, INF));
		vvi nexts(N, vi(N, -1));
		for (int i = 0; i < N; ++i) {
			dijkstra(i, neib, dists, nexts);
		}
		return make_pair(dists, nexts);
	}

	void init_graph(const Game &g) {
		N = g.planets.size();
		dists_line.resize(N, vi(N));
		for (int a = 0; a < N; ++a) {
			for (int b = 0; b < N; ++b) {
				dists_line[a][b] = mabs(g.planets[a].x - g.planets[b].x) +
								   mabs(g.planets[a].y - g.planets[b].y);
			}
		}
		int max_any = g.maxTravelDistance;
		int max_fast = max_any + g.logisticsUpgrade;
		tie(dists_any, nexts_any) = calc_all(max_any);
		tie(dists_fast, nexts_fast) = calc_all(max_fast);
	}
};  // namespace graph
