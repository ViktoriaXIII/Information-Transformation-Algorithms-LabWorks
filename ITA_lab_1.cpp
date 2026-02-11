#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

static const string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";

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
