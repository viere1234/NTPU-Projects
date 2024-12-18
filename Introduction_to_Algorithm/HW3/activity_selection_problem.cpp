#include <algorithm>
#include <iostream>
#include <climits>
#include <vector>
#include <queue>
using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    struct Activity {
        int s, f, num;
    };

    // Input
    int n;
    cin >> n;
    vector<Activity> activities(n);
    for (int i=0;i<n;++i) {
        cin >> activities[i].s;
        activities[i].num = i + 1;
    }
    for (int i=0;i<n;++i)
        cin >> activities[i].f;

    // Compute
    sort(activities.begin(), activities.end(), [](const auto& lhs, const auto& rhs) { return lhs.f < rhs.f; });
    vector<int> ans = { activities[0].num };
    for (int i=1, j=0;i<n;++i)
        if (activities[i].s >= activities[j].f) {
            ans.push_back(activities[i].num);
            j = i;
        }

    // Output
    cout << ans.size() << "\n(";
    for (int i=0;i<ans.size();++i) {
        if (i) cout << ',';
        cout << ans[i];
    }
    cout << ')';

    return 0;
}

