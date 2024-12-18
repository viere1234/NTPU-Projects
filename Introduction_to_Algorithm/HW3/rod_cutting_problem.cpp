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
    int k;
    cin >> k;
    vector<int> p(k);
    for (int i=0;i<k;++i)
        cin >> p[i];

    // Compute
    vector<int> r(k+1), s(k+1);
    r[0] = 0;
    for (int i=1;i<=k;++i) {
        int q = INT_MIN, tmp;
        for (int j=1;j<=i;++j) {
            tmp = p[j-1] + r[i-j];
            if (q < tmp) {
                q = tmp;
                s[i] = j;
            }
        }
        r[i] = q;
    }

    // Output
    cout << r[k] << '\n';
    vector<int> tmp;
    int n = k;
    while (n > 0) {
        tmp.push_back(s[n]);
        n -= s[n];
    }
    cout << tmp.size() << '\n' << k << '=';
    for (int i=0;i<tmp.size();++i) {
        if (i) cout << '+';
        cout << tmp[i];
    }


    return 0;
}
