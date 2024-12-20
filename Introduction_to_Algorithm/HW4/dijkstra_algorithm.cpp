#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, m, tmp_i, tmp_j;
    float w;
    cin >> n >> m;
    vector<vector<pair<int, float>>> adjList(n);
    vector<bool> visited(n, false);
    for (int i=0;i<m;++i) {
        cin >> tmp_i >> tmp_j >> w;
        adjList[tmp_i-1].emplace_back(tmp_j-1, w);
    }

    priority_queue<pair<float, int>, vector<pair<float, int>>> pq;
    cin >> tmp_i;
    pq.emplace(0, tmp_i-1);

    vector<float> dist(n, numeric_limits<float>::max());
    vector<int> parent(n, -1);
    dist[tmp_i-1] = 0;
    parent[tmp_i-1] = 0;

    while (!pq.empty()) {
        const auto e = pq.top();
        pq.pop();

        for (const auto& v : adjList[e.second]) {
            float new_dist = dist[e.second] + v.second;
            if (new_dist < dist[v.first]) {
                dist[v.first] = new_dist;
                parent[v.first] = e.second + 1;
                pq.emplace(new_dist, v.first);
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
