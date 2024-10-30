#include <bits/stdc++.h>
using namespace std;

void radixSort() {
    int n, k;
    cin >> n >> k;
    vector<int> count(10), countSum(10, 0);
    vector<string> arr(n), tmp(n);
    for (int i=0;i<n;++i)
        cin >> arr[i];

    // Radix Sort
    for (int i=k-1;i>=0;--i) {
        // Counting Sort
        count = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        for (int j=0;j<n;++j)
            ++count[arr[j][i] - '0'];
        for (int j=1;j<10;++j)
            count[j] += count[j-1];
        for (int j=n-1;j>=0;--j)
            tmp[--count[arr[j][i] - '0']] = arr[j];

        arr = tmp;
        for (int j=0;j<10;++j)
            countSum[j] += count[j];
    }

    for (int i=0;i<n;++i) {
        if (i) cout << ' ';
        cout << arr[i];
    }
    cout << '\n';
    for (int i=0;i<10;++i) {
        if (i) cout << ' ';
        cout << countSum[i];
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    radixSort();
}
