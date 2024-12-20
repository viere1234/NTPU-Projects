#include <bits/stdc++.h>
using namespace std;

int main() {
    // Input
    struct Edge {
        int u, v;
        float w;
    };
    int n, m;
    cin >> n >> m;
    vector<Edge> edges(m);
    for (int i=0;i<m;++i) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
        --edges[i].u, --edges[i].v;
    }

    // Calculate h
    bool negativeCycle = false; // Future-proof
    vector<float> bellman_dist(n+1, numeric_limits<float>::max());
    bellman_dist[n] = 0;
    vector<Edge> bellman_edges(edges);
    for (int i=0;i<n;++i)
        bellman_edges.push_back({n, i, 0});
    for (int i=0;i<n+1;++i)
        for (const auto& edge: bellman_edges)
            if (bellman_dist[edge.u] != numeric_limits<float>::max() && bellman_dist[edge.u] + edge.w < bellman_dist[edge.v]) {
                negativeCycle = i == n;
                bellman_dist[edge.v] = bellman_dist[edge.u] + edge.w;
            }
    vector<float> hs(bellman_dist.begin(),  bellman_dist.end() - 1);
    for (int i=0;i<n;++i) {
        if (i) cout << ' ';
        cout << hs[i];
    }
    cout << "\n\n";

    // Reweight
    vector<vector<pair<int, float>>> adjList(n);
    for (int i = 0; i < m; ++i)
        adjList[edges[i].u].push_back({edges[i].v, edges[i].w + hs[edges[i].u] - hs[edges[i].v]});

    // Dijkstra
    for (int i=0;i<n;++i) {
        priority_queue<pair<float, int>, vector<pair<float, int>>> pq;
        pq.emplace(0, i);

        vector<float> dist(n, numeric_limits<float>::max());
        dist[i] = 0;

        while (!pq.empty()) {
            const auto e = pq.top();
            pq.pop();
            for (const auto& v : adjList[e.second]) {
                float new_dist = dist[e.second] + v.second;
                if (new_dist < dist[v.first]) {
                    dist[v.first] = new_dist;
                    pq.emplace(new_dist, v.first);
                }
            }
        }

        if (i) cout << '\n';
        for (int j=0;j<n;++j) {
            if (j) cout << ' ';
            if (dist[j] == numeric_limits<float>::max())
                cout << 'N';
            else
                cout << dist[j] - hs[i] + hs[j];
        }
    }
}
