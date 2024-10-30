#include <bits/stdc++.h>
using namespace std;

void innerHash() {
    int m, r, i=0;
    cin >> m >> r;
    vector<int> a(r+1), k(r+1);
    string s, num;
    cin >> s;
    s = s.substr(1, s.size() - 2);
    stringstream ss(s);
    while (getline(ss, num, ','))
        a[i++] = stoi(num);
    i = 0;
    cin >> s;
    s = s.substr(1, s.size() - 2);
    ss = stringstream(s);
    while (getline(ss, num, ','))
        k[i++] = stoi(num);

    long long int sum = 0;
    for (i=0;i<=r;++i)
        sum += a[i] * k[i];
    cout << sum % m;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    innerHash();
}
