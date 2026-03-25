#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>

using namespace std;

struct Node {
    char leef;
    int freq;
    Node *left, *right;
};

Node* getNode(char leef, int freq, Node* left, Node* right) {
    Node* node = new Node();
    node->leef = leef;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

struct forComp { // Для впорядкування кучі
    bool operator () (Node* l, Node* r) {
        return l->freq > r->freq; // Більша пріоритетність => менша частота
    }
};

void encodeHuffman(Node* root, string str, unordered_map <char, string>& huffmanCode) {
    if (root == nullptr) return;
    if (!root->left && !root->right) { // Знайшли листок?
        huffmanCode[root->leef] = str;
    }
    encodeHuffman(root->left, str + "0", huffmanCode);
    encodeHuffman(root->right, str + "1", huffmanCode);
}

void decodeHuffman(Node* root, int& index, string str) {
    if (root == nullptr) return;
    if (!root->left && !root->right) { // Знайшли листок?
        cout << root->leef;
    }
    index++;
    if (str[index] == '0') decodeHuffman(root->left, index, str);
    else decodeHuffman(root->right, index, str);
}

int main()
{
    cout << "Hello World!\n";
}