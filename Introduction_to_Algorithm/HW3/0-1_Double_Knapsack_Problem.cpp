#include <algorithm>
#include <iostream>
#include <climits>
#include <vector>
#include <queue>
using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    // Input
    int n, w, v;
    cin >> n >> w >> v;
    vector<int> cost(n);
    vector<int> weight(n);
    vector<int> volume(n);
    for (int i=0;i<n;++i)
        cin >> cost[i];
    for (int i=0;i<n;++i)
        cin >> weight[i];
    for (int i=0;i<n;++i)
        cin >> volume[i];

    // Compute
    int tmp;
    vector<vector<int>> dp(w+1, vector<int>(v+1, 0));
    vector<vector<vector<bool>>> pick(n, vector<vector<bool>>(w+1, vector<bool>(v+1, false)));
    for (int i=0;i<n;++i) // Items
        for (int j=w;j>=weight[i];--j)
            for (int k=v;k>=volume[i];--k) {
                tmp = dp[j-weight[i]][k-volume[i]] + cost[i];
                if (tmp > dp[j][k]) {
                    dp[j][k] = tmp;
                    pick[i][j][k] = true;
                }
            }

    // Output
    cout << dp[w][v] << '\n';
    vector<int> list;
    for (int i=n-1, j=w, k=v;i>=0;--i)
        if (pick[i][j][k]) {
            list.push_back(i+1);
            j -= weight[i];
            k -= volume[i];
        }
    cout << list.size() << "\n(";
    for (int i=list.size()-1;i>=0;--i) {
        cout << list[i];
        if (i) cout << ',';
    }
    cout << ')';

    return 0;
}
