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
            return input_file.substr(0, input_file.length() - extension.length());
        }
        else return "decompressed_" + input_file;
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

void decompress(const string& input_file, const string& output_file) {}

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