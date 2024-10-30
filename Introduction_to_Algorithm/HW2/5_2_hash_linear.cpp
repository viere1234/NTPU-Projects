#include <bits/stdc++.h>
using namespace std;

void linearHash() {
    long long int p, m, a, b, k;
    cin >> p >> m >> a >> b >> k;
    cout << ((a * k + b) % p) % m;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    linearHash();
}
