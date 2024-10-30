#include <iostream>
#include <vector>

using namespace std;

int partition(vector<float>& arr, int left, int right, int& cmpCount) {
    const float pivot = arr[left];
    int i = left;
    for (int j = left + 1; j <= right; ++j) {
        ++cmpCount;
        if (arr[j] <= pivot)
            swap(arr[++i], arr[j]);
    }
    swap(arr[left], arr[i]);
    return i;
}

void quickSort(vector<float>& arr, const int left, const int right, int& divideCount, int& cmpCount) {
    if (left < right) {
        const int pi = partition(arr, left, right, cmpCount);
        ++divideCount;
        quickSort(arr, left, pi - 1, divideCount, cmpCount);
        quickSort(arr, pi + 1, right, divideCount, cmpCount);
    }
}

void quick_sort() {
    int n, divideCount = 0, cmpCount = 0;
    cin >> n;
    vector<float> arr(n);
    for (int i=0;i<n;++i)
        cin >> arr[i];

    quickSort(arr, 0, n - 1, divideCount, cmpCount);

    for (int i=0;i<n;++i) {
        if (i) cout << " ";
        cout << arr[i];
    }
    cout << "\n" << divideCount << "\n" << cmpCount;
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    quick_sort();

    return 0;
}
