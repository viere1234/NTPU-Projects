#include <iostream>
#include <vector>

using namespace std;

void merge(vector<float>& arr, const int left, const int mid, const int right, int& cmpCount) {
    const int n = right - left + 1;
    vector<float> tmp;
    tmp.reserve(n);

    int i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        ++cmpCount;
        if (arr[i] <= arr[j])
            tmp.push_back(arr[i++]);
        else
            tmp.push_back(arr[j++]);
    }

    while (i <= mid)
        tmp.push_back(arr[i++]);

    while (j <= right)
        tmp.push_back(arr[j++]);

    for (i = 0; i < n; ++i)
        arr[left + i] = tmp[i];
}

void mergeSort(vector<float>& arr, const int left, const int right, int& divideCount, int& cmpCount) {
    if (left >= right) return;
    const int mid = (left + right - 1) / 2;
    ++divideCount;
    mergeSort(arr, left, mid, divideCount, cmpCount);
    mergeSort(arr, mid + 1, right, divideCount, cmpCount);
    merge(arr, left, mid, right, cmpCount);
}

void merge_sort() {
    int n, count=0;

    // Input
    cin >> n;
    vector<float> arr(n);
    for (int i=0;i<n;++i)
        cin >> arr[i];

    int divideCount=0, cmpCount=0;
    mergeSort(arr, 0, arr.size() - 1, divideCount, cmpCount);

    // Output
    for (int i=0;i<n;++i) {
        if (i) cout << " ";
        cout << arr[i];
    }
    cout << "\n" << divideCount << "\n" << cmpCount;
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    merge_sort();

    return 0;
}
