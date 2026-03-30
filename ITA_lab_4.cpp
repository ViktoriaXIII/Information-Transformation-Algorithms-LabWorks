#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>

using namespace std;

struct Node {
    uint8_t leaf;
    int freq;
    shared_ptr<Node> left, right;
    Node (uint8_t l, int f) : leaf(l), freq(f), left(nullptr), right(nullptr) {}
    Node (shared_ptr<Node> left, shared_ptr<Node> right) : leaf(0), freq(left->freq + right->freq), left(left), right(right) {}
};

struct compare {
    bool operator()(const shared_ptr<Node>& l, const shared_ptr<Node>& r) const {
        if (l->freq == r->freq) return l->leaf > r->leaf;
        return l->freq > r->freq;
    }
};

void createCodes(shared_ptr<Node> root, const string& prefix, unordered_map<uint8_t, string>& HuffmanCode) {
    if (!root->left && !root->right) {
        HuffmanCode[root->leaf] = prefix.empty() ? "0" : prefix;
        return;
    }
    createCodes(root->left, prefix + "0", HuffmanCode);
    createCodes(root->right, prefix + "1", HuffmanCode);
}

void buildTree(shared_ptr<Node> node, ostream& out) {
    if (!node->left && !node->right) {
        uint8_t flag = 1;
        out.write((char*)&flag, 1);
        out.write((char*)&node->leaf, 1);
    }
    else {
        uint8_t flag = 0;
        out.write((char*)&flag, 1);
        buildTree(node->left, out);
        buildTree(node->right, out);
    }
}

shared_ptr<Node> readTree(istream& in) {
    uint8_t flag;
    in.read((char*)&flag, 1);
    if (flag == 1) {
        uint8_t b;
        in.read((char*)&b, 1);
        return make_shared<Node>(b, 0);
    }
    auto left = readTree(in);
    auto right = readTree(in);
    return make_shared<Node>(left, right);
}

class WriteBits {
    ostream& out;
    uint8_t buffer = 0;
    int count = 0;
public:
    WriteBits(ostream& os) : out(os) {}
    void writeBit(int bit) {
        buffer |= (bit & 1) << (7 - count);
        if (++count == 8) {
            out.put(buffer);
            buffer = 0;
            count = 0;
        }
    }
    void writeCode(const string& s) {
        for (char c : s) writeBit(c == '1');
    }
    void flush() {
        if (count > 0) out.put(buffer);
    }
};

class ReadBits {
    istream& in;
    uint8_t buffer = 0;
    int count = 8;
public: 
    ReadBits(istream& is) : in(is) {}
    int readBit() {
        if (count == 8) {
            if (!in.read((char*)&buffer, 1)) return -1;
            count = 0;
        }
        return (buffer >> (7 - count++)) & 1;
    }
};

void compressFile(const string& inPath, const string& outPath) {
    ifstream in(inPath, ios::binary);
    if (!in) return;
    unordered_map<uint8_t, int> freq;
    vector<uint8_t> data;
    uint8_t b;
    while (in.read((char*)&b, 1)) {
        freq[b]++;
        data.push_back(b);
    }
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, compare> queue;
    for (auto& p : freq) {
        queue.push(make_shared<Node>(p.first, p.second));
    }
    if (queue.empty()) return;
    while (queue.size() > 1) {
        auto l = queue.top(); queue.pop();
        auto r = queue.top(); queue.pop();
        queue.push(make_shared<Node>(l, r));
    }
    auto root = queue.top();
    unordered_map<uint8_t, string> codes;
    createCodes(root, "", codes);
    ofstream out(outPath, ios::binary);
    buildTree(root, out);
    uint32_t total = data.size();
    out.write((char*)&total, sizeof(total));
    uint32_t bitCount = 0;
    for (auto x : data) bitCount += codes[x].size();
    out.write((char*)&bitCount, sizeof(bitCount));
    WriteBits bw(out);
    for (uint8_t x : data) bw.writeCode(codes[x]);
    bw.flush();
}

void decompressFile(const string& inPath, const string& outPath) {
    ifstream in(inPath, ios::binary);
    if (!in) return;
    auto root = readTree(in);
    uint32_t total;
    in.read((char*)&total, sizeof(total));
    uint32_t bitCount;
    in.read((char*)&bitCount, sizeof(bitCount));
    ofstream out(outPath, ios::binary);
    ReadBits rb(in);
    uint32_t readBits = 0;
    auto node = root;
    while (readBits < bitCount) {
        int bit = rb.readBit();
        if (bit < 0) break;
        readBits++;
        node = bit ? node->right : node->left;
        if (!node->left && !node->right) {
            out.put(node->leaf);
            node = root;
            if (--total = 0) break;
        }
    }
}

int main()
{
    compressFile("example.bmp", "example_bmp.huf");
    decompressFile("example_bmp.huf", "example_decompressed.bmp");
    compressFile("example.txt", "example_txt.huf");
    decompressFile("example_txt.huf", "example_decompressed.txt");
}