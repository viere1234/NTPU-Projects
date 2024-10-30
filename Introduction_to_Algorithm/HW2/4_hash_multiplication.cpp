#include <bits/stdc++.h>
using namespace std;

void multiplicationHash() {
    int w, r;
    string A, k;
    cin >> w >> r >> A >> k;
    reverse(A.begin(), A.end());
    reverse(k.begin(), k.end());

    string Ak(2*w, '0');
    for (int i=0;i<w;++i) {
        if (k[i] == '0') continue;
        bool carry = false;
        for (int j=0;j<w;++j) {
            int sum = (Ak[i + j] - '0') + (A[j] - '0') + carry;
            Ak[i + j] = sum % 2 ? '1' : '0';
            carry = sum / 2;
        }
        Ak[w + i] = carry ? '1' : '0';
    }

    int i = 2 * w - 1;
    while (Ak[i] == '0' && i > 0) --i;
    if (i) while (i >= 0) cout << Ak[i--];
    else cout << Ak[0];
    cout << '\n';

    for (i = w - 1; i >= w - r; i--)
        cout << Ak[i];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    multiplicationHash();
}
