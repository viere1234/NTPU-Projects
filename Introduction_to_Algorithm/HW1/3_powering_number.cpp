#include <iostream>
#include <vector>

using namespace std;

long long int poweringNumber(const int& a, const int n, int& count) {
    if (n == 0) return 1;
    if (n == 1) return a;
    const long long int tmp = poweringNumber(a, n / 2, count);
    ++count;
    if (n % 2 == 0) return tmp * tmp;
    ++count;
    return tmp * tmp * a;
}

void powering_number() {
    int a, n, count=0;
    cin >> a >> n;
    cout << poweringNumber(a, n, count) << "\n";
    cout << count;
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    powering_number();

    return 0;
}
