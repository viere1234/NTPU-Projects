#include <iostream>
#include <vector>

using namespace std;

vector<vector<int>> mulMatrix(const vector<vector<int>>& matrix1, const vector<vector<int>>& matrix2) {
    const int rows = matrix1.size(), columns = matrix2[0].size(), inner = matrix2.size();
    vector<vector<int>> result(rows, vector<int>(columns, 0));
    for (int i=0;i<rows;++i)
        for (int j=0;j<columns;++j)
            for (int k=0;k<inner;++k)
                result[i][j] += matrix1[i][k] * matrix2[k][j];
    return result;
}

vector<vector<int>> poweringMatrix(const vector<vector<int>>& matrix, const int n, int& count) {
    if (n == 1) return matrix;
    vector<vector<int>> tmp = poweringMatrix(matrix, n / 2, count);
    ++count;
    if (n % 2 == 0) return mulMatrix(tmp, tmp);
    ++count;
    return mulMatrix(mulMatrix(tmp, tmp), matrix);
}

void fibonacci_numbers() {
    // Input
    int k, m, count=0;
    cin >> k;
    vector<int> baseVector(k);
    for (int i=0;i<k;++i)
        cin >> baseVector[i];
    vector<vector<int>> matrix(k, vector<int>(k, 0));
    for (int i=0;i<k;++i)
        cin >> matrix[0][i];
    cin >> m;

    // Base case
    if (m <= k) cout << baseVector[m-1] << "\n0";

    for (int i=1;i<k;++i)
        matrix[i][i-1] = 1;

    vector<vector<int>> tmp = poweringMatrix(matrix, m - k, count);

    int result = 0;
    for (int i=0;i<k;++i)
        result += tmp[0][i] * baseVector[k - i - 1];
    cout << result << "\n" << count;
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    fibonacci_numbers();

    return 0;
}
