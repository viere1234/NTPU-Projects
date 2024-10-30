#include <iostream>
#include <vector>
using namespace std;

struct Node {
    int key;
    Node* left;
    Node* right;
};

Node* createNode(const int& key) {
    Node* node = new Node();
    node->key = key;
    node->right = node->left = nullptr;
    return node;
}

Node* insertKey(Node* node, const int& key) {
    if (node == nullptr) return createNode(key);
    if (key < node->key) node->left = insertKey(node->left, key);
    else node->right = insertKey(node->right, key);
    return node;
}

void inorderTraversal(vector<int>& arr, Node* node) {
    if (node == nullptr) return;
    inorderTraversal(arr, node->left);
    arr.push_back(node->key);
    inorderTraversal(arr, node->right);
}

void searchKey(Node* node, const int& key, vector<int>& path) {
    if (node == nullptr) return;
    path.push_back(node->key);
    if (node->key == key) return;
    if (key < node->key) searchKey(node->left, key, path);
    else searchKey(node->right, key, path);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k, tmp;
    Node* root = nullptr;
    cin >> n >> k;
    for (int i=0;i<n;++i) {
        cin >> tmp;
        root = insertKey(root, tmp);
    }

    vector<int> arr, path;
    arr.reserve(n);
    path.reserve(n);
    inorderTraversal(arr, root);
    searchKey(root, arr[k-1], path);

    cout << path.size() << '\n';
    for (int i=0;i<path.size();++i) {
        if (i) cout << ' ';
        cout << path[i];
    }
}

