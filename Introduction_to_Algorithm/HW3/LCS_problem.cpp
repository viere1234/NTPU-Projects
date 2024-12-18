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
    string s;
    cin >> s;
    int m, n;
    cin >> m >> n;
    vector<char> x(m), y(n);
    for (int i=0;i<m;++i)
        cin >> x[i];
    for (int i=0;i<n;++i)
        cin >> y[i];

    // Compute
    vector<vector<int>> c(m+1, vector<int>(n+1, 0));
    vector<vector<int>> direction(m, vector<int>(n));
    for (int i=1;i<=m;++i)
        for (int j=1;j<=n;++j)
            if (x[i-1] == y[j-1]) {
                c[i][j] = c[i-1][j-1] + 1;
                direction[i-1][j-1] = 1;
            } else if (c[i][j-1] == c[i-1][j]) {
                c[i][j] = c[i-1][j];
                direction[i-1][j-1] = 3;
            } else if (c[i][j-1] > c[i-1][j]) {
                c[i][j] = c[i][j-1];
                direction[i-1][j-1] = 2;
            } else {
                c[i][j] = c[i-1][j];
                direction[i-1][j-1] = 3;
            }

    // Output
    cout << c[m][n] << '\n';
    vector<char> sequence;
    int i=m-1, j=n-1;
    while (i >= 0 && j >= 0)
        switch (direction[i][j]) {
            case 1:
                sequence.push_back(x[i]);
                --i;
                --j;
                break;
            case 2:
                --j;
                break;
            case 3:
                --i;
        }
    for (i=c[m][n]-1;i>=0;--i) {
        cout << sequence[i];
        if (i) cout << ' ';
    }

    return 0;
}
