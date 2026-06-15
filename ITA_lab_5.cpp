#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <cstdint>
using namespace std;

string generate_output_filename(const string& input_file, const string& command) {
    if (command == "compress") return input_file + ".lzw";
    else if (command == "decompress") {
        string extension = ".lzw";
        if (input_file.length() >= extension.length() &&
            input_file.compare(input_file.length() - extension.length(), extension.length(), extension) == 0) {
            return input_file.substr(0, input_file.length() - extension.length()) + ".dec";
        }
        else return input_file + ".dec";
    }
    return "";
}

void compress(const string& input_file, const string& output_file) {
    ifstream in(input_file, ios::binary);
    ofstream out(output_file, ios::binary);
    if (!in || !out) {
        cout << "File error\n";
        return;
    }
    uint16_t max_dict_size = 65535;
    uint8_t mode = 1;
    out.write((char*)&max_dict_size, sizeof(max_dict_size));
    out.write((char*)&mode, sizeof(mode));
    unordered_map<string, uint16_t> dict;
    for (int i = 0; i < 256; i++) dict[string(1, char(i))] = i;
    uint16_t dict_size = 256;
    string S = "";
    char C;
    while (in.get(C)) {
        string SC = S + C;
        if (dict.count(SC)) S = SC;
        else {
            if (!S.empty()) {
                uint16_t I = dict[S];
                out.write((char*)&I, sizeof(I));
            }

            if (dict_size < max_dict_size) dict[SC] = dict_size++;
            else {
                if (mode == 1) {
                    dict.clear();
                    for (int i = 0; i < 256; i++) dict[string(1, char(i))] = i;
                    dict_size = 256;
                }
            }
            S = string(1, C);
        }
    }
    if (!S.empty()) {
        uint16_t I = dict[S];
        out.write((char*)&I, sizeof(I));
    }
    cout << "Compressed successfully!\n";
}

void decompress(const string& input_file, const string& output_file) {
    ifstream in(input_file, ios::binary);
    ofstream out(output_file, ios::binary);
    if (!in || !out) {
        cout << "File error\n";
        return;
    }
    uint16_t max_dict_size;
    uint8_t mode;
    if (!in.read((char*)&max_dict_size, sizeof(max_dict_size)) || !in.read((char*)&mode, sizeof(mode))) {
        cout << "Invalid file format\n";
        return;
    }
    vector<string> dict;
    for (int i = 0; i < 256; i++) {
        dict.push_back(string(1, char(i)));
    }
    uint16_t prev_code;
    if (!in.read((char*)&prev_code, sizeof(prev_code))) {
        cout << "Empty file\n";
        return;
    }
    if (prev_code >= dict.size()) {
        cout << "Corrupted file\n";
        return;
    }
    string S = dict[prev_code];
    out << S;
    uint16_t code;
    while (in.read((char*)&code, sizeof(code))) {
        string entry;
        if (code < dict.size()) entry = dict[code];
        else if (code == dict.size()) entry = S + S[0];
        else {
            cout << "Decoding error\n";
            return;
        }
        out << entry;
        if (dict.size() < max_dict_size) dict.push_back(S + entry[0]);
        else {
            if (mode == 1) {
                dict.clear();
                for (int i = 0; i < 256; i++) dict.push_back(string(1, char(i)));
            }
        }
        S = entry;
    }
    cout << "Decompressed successfully!\n";
}

int main(){
    string command;
    while (true) {
        cout << "Enter command " << "(compress/decompress/stop): ";
        cin >> command;
        if (command == "stop") break;
        string input_file;
        cout << "Input file: ";
        cin >> input_file;
        string output_file = generate_output_filename(input_file, command);
        if (command == "compress") compress(input_file, output_file);
        else if (command == "decompress") decompress(input_file, output_file);
        else cout << "Unknown command\n";
    }
}