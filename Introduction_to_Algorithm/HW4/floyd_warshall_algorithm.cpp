#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, m, u, v;
    float w;
    cin >> n >> m;
    vector<vector<float>> dist(n, vector<float>(n, numeric_limits<float>::max()));
    for (int i=0;i<n;++i)
        dist[i][i] = 0;
    for (int i=0;i<m;++i) {
        cin >> u >> v >> w;
        dist[u-1][v-1] = w;
    }

    for (int k=0;k<n;++k)
        for (int i=0;i<n;++i)
            for (int j=0;j<n;++j)
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);

    for (int i=0;i<n;++i) {
        if (i) cout << '\n';
        for (int j = 0; j < n; ++j) {
            if (j) cout << ' ';
            if (dist[i][j] != numeric_limits<float>::max())
                cout << dist[i][j];
            else
                cout << 'N';
        }
    }
}
