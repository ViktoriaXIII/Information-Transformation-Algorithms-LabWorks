#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <iterator>
using namespace std;

void encode_homo(ostream& out, uint8_t byte, int length) {
    // L = 128 + l - 2 => 126 + l
    uint8_t L = static_cast<uint8_t>(126 + length);
    out.put(static_cast<char>(L));
    out.put(static_cast<char>(byte));
}

void encode_hetero(ostream& out, const vector<uint8_t>& buffer) {
    if (buffer.empty()) return;
    uint8_t L = static_cast<uint8_t>(buffer.size() - 1);
    out.put(static_cast<char>(L));
    out.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

void encode_RLE(const string& in_f, const string& out_f) {
    ifstream in(in_f, ios::binary | ios::ate);
    if (!in) { cerr << "Error opening input!\n"; return; }
    streamsize size = in.tellg();
    in.seekg(0, ios::beg);
    vector<uint8_t> data(size);
    in.read(reinterpret_cast<char*>(data.data()), size);
    in.close();
    ofstream outFile(out_f, ios::binary);
    if (!outFile) { cerr << "Error opening output!\n"; return; }
    vector<uint8_t> hetero_buffer;
    size_t i = 0;
    while (i < data.size()) {
        size_t run_len = 1;
        while (i + run_len < data.size() && data[i + run_len] == data[i] && run_len < 129) {
            run_len++;
        }
        if (run_len >= 2) {
            if (!hetero_buffer.empty()) {
                encode_hetero(outFile, hetero_buffer);
                hetero_buffer.clear();
            }
            encode_homo(outFile, data[i], (int)run_len);
            i += run_len;
        }
        else {
            hetero_buffer.push_back(data[i]);
            i++;
            if (hetero_buffer.size() == 128) {
                encode_hetero(outFile, hetero_buffer);
                hetero_buffer.clear();
            }
        }
    }
    if (!hetero_buffer.empty()) {
        encode_hetero(outFile, hetero_buffer);
    }
    outFile.close();
}

void decode_RLE(const string& in_f, const string& out_f) {
    ifstream in(in_f, ios::binary);
    if (!in) {
        cerr << "Error: Could not open encoded file!" << endl;
        return;
    }
    ofstream out(out_f, ios::binary);
    if (!out) {
        cerr << "Error: Could not open output file!" << endl;
        return;
    }
    char L_char;
    while (in.get(L_char)) {
        uint8_t L = static_cast<uint8_t>(L_char);
        if (L <= 127) {
            // Гетеро-блок
            int count = static_cast<int>(L) + 1;
            vector<char> buffer(count);
            in.read(buffer.data(), count);
            if (in.gcount() < count) { // Чи достатньо байтів для L?
                cerr << "\nError!!! Corrupted RLE header. " << "Expected " << count << " bytes, but found only " << in.gcount() << "." << endl;
                break;
            }
            out.write(buffer.data(), count);
        }
        else {
            // Гомо-блок
            int count = static_cast<int>(L) - 126;
            char byte_to_repeat;
            if (!in.get(byte_to_repeat)) { // Чи існує байтвзагалі?
                cerr << "\nError!!! Unexpected end of file after homo-command." << endl;
                break;
            }
            for (int j = 0; j < count; ++j) {
                out.put(byte_to_repeat);
            }
        }
    }
    in.close();
    out.close();
}

int main(){
    int mode, name;
    string in_f, out_f;
    cout << "1. Encode the file (RLE)\n2. Decode RLE-file\nChoice: ";
    cin >> mode;
    cout << "Enter the name of the input file: ";
    cin >> in_f;
    cout << "\nHow to name output file?\n1. To enter manually\n2. To create automatically\nChoice: ";
    cin >> name;
    if (name == 1) {
        cout << "Enter the name of the output file: ";
        cin >> out_f;
    }
    else {
        if (mode == 1) {
            out_f = in_f + ".rle";
        }
        else {
            out_f = in_f;
            if (out_f.size() > 4 && out_f.substr(out_f.size() - 4) == ".rle") {
                out_f.erase(out_f.size() - 4);
            }
            size_t dot_pos = out_f.find_last_of(".");
            if (dot_pos != string::npos) {
                out_f.insert(dot_pos, "_decoded");
            }
            else {
                out_f += "_decoded";
            }
        }
    }
    if (in_f == out_f) {
        cerr << "Error: Output file name would be same as input! Operation aborted." << endl;
        return 1;
    }
    if (mode == 1) {
        encode_RLE(in_f, out_f);
        cout << "\nEncoded file is done! Saved as: " << out_f << endl;
    }
    else if (mode == 2) {
        decode_RLE(in_f, out_f);
        cout << "\nDecoded file is done! Saved as: " << out_f << endl;
    }
    else {
        cout << "Wrong mode!\n";
    }
}
