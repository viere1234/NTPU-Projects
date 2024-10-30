#include <bits/stdc++.h>
using namespace std;

int BFPRT(vector<int>& arr, int l, int r, int k, int& d, int& s) {
    // Base case
    if (r - l + 1 <= 5) {
        ++s;
        vector<int> newArr(arr.begin() + l, arr.begin() + l + (r - l + 1));
        sort(newArr.begin(), newArr.end());
        return newArr[k-1];
    }

    // Find median of medians
    int n = (r - l + 1) / 5;
    vector<int> medians(n);
    for (int i=0;i<n;++i) {
        ++s;
        vector<int> newArr(arr.begin() + l + (i * 5), arr.begin() + l + (i * 5) + 5);
        sort(newArr.begin(), newArr.end());
        medians[i] = newArr[2];
    }
    ++d;
    //int pivot = BFPRT(medians, 0, medians.size() - 1, (medians.size() + 1) / 2, d, s);
    sort(medians.begin(), medians.end());
    int pivot = medians[(medians.size() + 1) / 2 - 1];

    // Partition
    ++s;
    for (int i=l;i<=r;++i)
        if (arr[i] == pivot) {
            swap(arr[i], arr[l]);
            break;
        }
    int i = l;
    for (int j = l+1;j<=r;++j)
        if (arr[j] <= arr[l])
            swap(arr[j], arr[++i]);
    swap(arr[l], arr[i]);
    i -= l;

    if (k == i + 1)
        return arr[l + i];
    ++d;
    if (k < i + 1)
        return BFPRT(arr, l, i + l - 1, k, d, s);
    return BFPRT(arr, i + l + 1, r, k - i - 1, d, s);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k, b, d = 0, s = 0;
    cin >> n >> k;
    vector<int> arr(n);
    for (int i=0;i<n;++i)
        cin >> arr[i];
    b = BFPRT(arr, 0, n - 1, k, d, s);
    cout << b << '\n' << d << '\n' << s;
}
