#include <bits/stdc++.h>
using namespace std;

void countingSort() {
    int n, k;
    cin >> n >> k;
    vector<int> arr(n), count(k+1, 0), result(n);
    for (int i=0;i<n;++i) {
        cin >> arr[i];
        ++count[arr[i]];
    }
    for (int i=1;i<=k;++i)
        count[i] += count [i-1];
    for (int i=n-1;i>=0;--i)
        result[--count[arr[i]]] = arr[i];

    for (int i=0;i<n;++i) {
        if (i) cout << ' ';
        cout << result[i];
    }
    cout << '\n';
    for (int i=0;i<=k;++i) {
        if (i) cout << ' ';
        cout << count[i];
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    countingSort();
}
