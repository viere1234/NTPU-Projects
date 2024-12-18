#include <algorithm>
#include <iostream>
#include <climits>
#include <vector>
#include <queue>
using namespace std;

struct HuffmanTreeNode {
    HuffmanTreeNode(int num, int frequency, HuffmanTreeNode* left=nullptr, HuffmanTreeNode* right=nullptr): num(num), frequency(frequency), left(left), right(right) {};
    int num, frequency;
    HuffmanTreeNode* left;
    HuffmanTreeNode* right;
};

void generateEncodeTable(const HuffmanTreeNode* root, string& code, vector<string>& encodeTable) {
    if (!root->left && !root->right) {
        encodeTable[root->num-1] = code;
        return;
    }
    if (root->left) {
        code.push_back('0');
        generateEncodeTable(root->left, code, encodeTable);
        code.pop_back();
    }
    if (root->right) {
        code.push_back('1');
        generateEncodeTable(root->right, code, encodeTable);
        code.pop_back();
    }
}

void releaseHuffmanTree(HuffmanTreeNode* root) {
    if (!root) return;
    releaseHuffmanTree(root->left);
    releaseHuffmanTree(root->right);
    delete root;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    // Input
    int n;
    cin >> n;
    vector<int> f(n);
    for (int i=0;i<n;++i)
        cin >> f[i];

    // Compute
    const auto frequencyCmp = [](HuffmanTreeNode* a, HuffmanTreeNode* b) { return a->frequency > b->frequency; };
    priority_queue<HuffmanTreeNode*, std::vector<HuffmanTreeNode*>, decltype(frequencyCmp)> nodeList(frequencyCmp);
    for (int i=0;i<n;++i) {
        auto* pointer = new HuffmanTreeNode(i+1, f[i]);
        nodeList.push(pointer);
    }
    while (nodeList.size() > 1) {
        HuffmanTreeNode* left = nodeList.top();
        nodeList.pop();
        HuffmanTreeNode* right = nodeList.top();
        nodeList.pop();
        auto* newNode = new HuffmanTreeNode(0, left->frequency + right->frequency, left, right);
        nodeList.push(newNode);
    }
    string code;
    vector<string> encodeTable(n);
    generateEncodeTable(nodeList.top(), code, encodeTable);
    releaseHuffmanTree(nodeList.top());

    // Output
    cout << n << '\n';
    for (int i=0;i<n;++i) {
        if (i) cout << ' ';
        cout << encodeTable[i];
    }

    return 0;
}
