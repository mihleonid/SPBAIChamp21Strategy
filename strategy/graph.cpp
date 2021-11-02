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
int fast_dist(int fr, int to) { return dists_any[fr][to]; }
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
void dijkstra(int from, const vvi& neib, vvi& d, vvi& nexts) {
  vector<char> used(N);
  d[from][from] = 0;
  nexts[from][from] = from;
  set<pair<int, int>> open;
  open.insert({0, from});
  while (open.size()) {
    auto p = *(open.begin());
    open.erase(p);
    if (used[p.second]) {
      continue;
    }
    used[p.second] = 1;
    for (int nei : neib[p.second]) {
      if (used[nei]) {
        continue;
      }
      if (rmin(d[from][nei], d[from][p.second] + line_dist(p.second, nei))) {
        nexts[nei][from] = p.second;
        open.insert({d[from][nei], nei});
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
void init_graph(const Game& g) {
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
