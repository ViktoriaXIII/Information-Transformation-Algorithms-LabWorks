#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <memory>

using namespace std;

struct Node {
    char leef;
    int freq;
    shared_ptr<Node> left, right;
    Node (unsigned char l, int f) : leef(l), freq(f), left(nullptr), right(nullptr) {}
    Node (int f, shared_ptr<Node> left, shared_ptr<Node> right) : leef(0), freq(f), left(left), right(right) {}
};

struct compare {
    bool operator()(shared_ptr<Node> l, shared_ptr<Node> r) {
        return l->freq > r->freq;
    }
};

void createCodes(shared_ptr<Node> root, string str, unordered_map<unsigned char, string>& HuffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) { // Знайшли листок?
        HuffmanCode[root->leef] = str;
    }
    createCodes(root->left, str + "0", HuffmanCode);
    createCodes(root->right, str + "1", HuffmanCode);
}

class WriteBits {
    ofstream out;
    unsigned char buffer = 0;
    int count = 0;
public:
    WriteBits(const string& path) : out(path, ios::binary) {}
    void writeBit(char bit) {
        buffer <<= 1;
        if (bit == '1') buffer |= 1;
        if (++count == 8) {
            out.put(buffer);
            buffer = 0;
            count = 0;
        }
    }
    void flush() {
        if (count > 0) out.put(buffer << (8 - count));
        out.close();
    }
};

void compressFile(string inPath, string outPath) {
    ifstream in(inPath, ios::binary);
    if (!in) return;
    unordered_map<unsigned char, int> freq;
    unsigned char leef;
    int total = 0;
    while (in.read(reinterpret_cast<char*>(&leef), 1)) {
        freq[leef]++;
        total++;
    }
    if (total == 0) return;
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, compare> queue;
    for (auto i = freq.begin(); i != freq.end(); i++) {
        unsigned char val = i->first;
        int count = i->second;
        queue.push(make_shared<Node>(val, count));
    }
    while (queue.size() > 1) {
        auto l = queue.top(); queue.pop();
        auto r = queue.top(); queue.pop();
        queue.push(make_shared<Node>(l->freq + r->freq, l, r));
    }
    unordered_map<unsigned char, string> codes;
    createCodes(queue.top(), "", codes);
    ofstream out(outPath, ios::binary);
    int mapSize = freq.size();
    out.write(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
    out.write(reinterpret_cast<char*>(&total), sizeof(total));
    for (auto const& pair : freq) {
        const unsigned char& val = pair.first;
        const int& count = pair.second;
        out.write(reinterpret_cast<const char*>(&val), 1);
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    }
    in.clear();
    in.seekg(0);
    WriteBits bw(outPath);
    while(in.read(reinterpret_cast<char*>(&leef), 1)) {
        string code = codes[leef];
        for (char b : code) {
            bw.writeBit(b);
        }
    }
    bw.flush();
    out.close();
    in.close();
}

void runHuffman(string input, string output, bool compress) {
    if (compress) {
        cout << "Compressing..." << endl;
        compressFile(input, output);
    }
    else {
        cout << "Decompressing..." << endl;
        //decompressFile(input, output);
    }
}

int main()
{
    runHuffman("example.bmp", "example_bmp.huf", 1);
    //runHuffman("example_bmp.huf", "example_decompressed.bmp", 0);
    //runHuffman("example.txt", "example_txt.huf", 1);
    //runHuffman("example_txt.huf", "example_decompressed.txt", 0);
}