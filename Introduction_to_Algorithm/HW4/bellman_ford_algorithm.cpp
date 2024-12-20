#include <bits/stdc++.h>
using namespace std;

int main() {
    struct Edge { int u, v; float w; };
    int n, m, tmp_i, tmp_j;
    float w;
    cin >> n >> m;
    vector<Edge> edges(n);
    for (int i=0;i<m;++i) {
        cin >> tmp_i >> tmp_j >> w;
        edges.push_back({tmp_i-1, tmp_j-1, w});
    }
    cin >> tmp_i;

    vector<float> dist(n, numeric_limits<float>::max());
    vector<int> parent(n, -1);
    dist[tmp_i-1] = 0;
    parent[tmp_i-1] = 0;

    for (int i=0;i<n;++i) {
        for (const auto& edge: edges) {
            if (dist[edge.u] != numeric_limits<float>::max() && dist[edge.u] + edge.w < dist[edge.v]) {
                if (i == n-1) {
                    cout << "There is a negative weight cycle in the graph";
                    return 0;
                }
                dist[edge.v] = dist[edge.u] + edge.w;
                parent[edge.v] = edge.u + 1;
            }
        }
    }

    for (int i=0;i<n;++i) {
        if (i) cout << ' ';
        cout << dist[i];
    }
    cout << '\n';
    for (int i=0;i<n;++i) {
        if (i) cout << ' ';
        cout << parent[i];
    }
}
