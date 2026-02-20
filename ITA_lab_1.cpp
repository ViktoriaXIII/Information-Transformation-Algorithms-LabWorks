#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

static const string b64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";

void encode_triplet(const unsigned char* triplet, char* target, int len) {
    // 1-й символ: перші 6 біт першого байта
    target[0] = b64_alphabet[(triplet[0] & 0xFC) >> 2];

    // 2-й символ: 2 останні біти 1-го байта + 4 перші біти 2-го байта
    target[1] = b64_alphabet[((triplet[0] & 0x03) << 4) | ((len > 1 ? (triplet[1] & 0xF0) >> 4 : 0))];

    // 3-й символ: 4 останні біти 2-го байта + 2 перші біти 3-го байта
    if (len > 1) {
        target[2] = b64_alphabet[((triplet[1] & 0x0F) << 2) | ((len > 2 ? (triplet[2] & 0xC0) >> 6 : 0))];
    }
    else {
        target[2] = '='; // Падінг <= був лише 1 байт
    }

    // 4-й символ: 6 останніх бітів 3-го байта
    if (len > 2) {
        target[3] = b64_alphabet[triplet[2] & 0x3F];
    }
    else {
        target[3] = '='; // Падінг <= було менше 3 байтів
    }
}

void encode_file(const string& input_path, const string& output_path) {
    ifstream in(input_path, ios::binary);
    ofstream out(output_path);

    if (!in || !out) {
        cerr << "ERROR!!! Source file does not open" << endl;
        return;
    }
}

void decode_file(const string& input_path, const string& output_path) {
    ifstream in(input_path);
    ofstream out(output_path, ios::binary);

    if (!in || !out) {
        cerr << "ERROR!!! Source file does not open" << endl;
        return;
    }
}

int main()
{
    int mode, name;
    string in_f, out_f;

    cout << "1. Encode the file in Base64\n2. Decode Base64-file\nChoice: ";
    cin >> mode;

    cout << "Enter the name of the input file: ";
    cin >> in_f;

    cout << "\nHow to name output file?\n";
    cout << "1. To enter manually\n";
    cout << "2. To create automatically (add.base64 or .bin)\n";
    cout << "Choice: ";
    cin >> name;

    if (name == 1) {
        cout << "Enter the name of the output file: ";
        cin >> out_f;
    }
    else {
        if (mode == 1) {
            out_f = in_f + ".base64";
        }
        else {
            out_f = in_f + ".bin";
        }
    }

    if (mode == 1) {
        encode_file(in_f, out_f);
        cout << "\nEncoded file is done!" << out_f << endl;
    }
    else if (mode == 2) {
        decode_file(in_f, out_f);
        cout << "\nDecoded file is done!" << out_f << endl;
    }
    else {
        cout << "Wrong mode!\n";
    }
}
