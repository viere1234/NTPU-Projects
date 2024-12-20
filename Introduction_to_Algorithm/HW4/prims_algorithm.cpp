#include <bits/stdc++.h>
using namespace std;

int main() {
    struct Edge { int u, t; float w; };
    int n, m, tmp_i, tmp_j;
    float w;
    cin >> n >> m;
    vector<vector<pair<int, float>>> adjList(n);
    vector<bool> visited(n, false);
    for (int i=0;i<m;++i) {
        cin >> tmp_i >> tmp_j >> w;
        --tmp_i, --tmp_j;
        adjList[tmp_i].emplace_back(tmp_j, w);
        adjList[tmp_j].emplace_back(tmp_i, w);
    }
    const auto cmp = [](const auto& lhs, const auto& rhs){ return lhs.w > rhs.w; };
    priority_queue<Edge, vector<Edge>, decltype(cmp)> pq(cmp);
    cin >> tmp_i;
    pq.push({-1, tmp_i-1, 0});

    float res = 0;
    vector<int> arr(n);

    while(!pq.empty()) {
        Edge p = pq.top();
        pq.pop();

        w = p.w;
        int u = p.u, t = p.t;

        if (visited[t]) continue;

        arr[t] = u + 1;
        res += w;
        visited[t] = true;

        for (const auto& v : adjList[t])
            if (!visited[v.first])
                pq.push({t, v.first, v.second});
    }

    for (int i=0;i<n;++i) {
        if (i) cout << ' ';
        cout << arr[i];
    }
    cout  << '\n' << res << endl;
}
