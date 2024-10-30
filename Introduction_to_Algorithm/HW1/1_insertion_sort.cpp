#include <iostream>
#include <vector>

using namespace std;

void insertion_sort() {
    int n, count=0;
    float tmp;

    // Input
    cin >> n;
    vector<float> arr(n);
    for (int i=0;i<n;++i)
        cin >> arr[i];

    // Sort
    for (int i=1, j; i<n; ++i) {
        tmp = arr[i];
        j = i - 1;
        while (arr[j] > tmp && j >= 0) {
            ++count;
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = tmp;
        if (j >= 0) ++count;
    }

    // Output
    for (int i=0;i<n;++i) {
        if (i) cout << " ";
        cout << arr[i];
    }
    cout << "\n" << count;
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    insertion_sort();

    return 0;
}
