#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

static const string b64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void encode_triplet(const unsigned char* triplet, char* target) {
    // 1-й символ: перші 6 біт першого байта
    target[0] = b64_alphabet[(triplet[0] & 0xFC) >> 2];
    // 2-й символ: 2 останні біти 1-го байта + 4 перші біти 2-го байта
    target[1] = b64_alphabet[((triplet[0] & 0x03) << 4) | ((triplet[1] & 0xF0) >> 4)];
    // 3-й символ: 4 останні біти 2-го байта + 2 перші біти 3-го байта
    target[2] = b64_alphabet[((triplet[1] & 0x0F) << 2) | ((triplet[2] & 0xC0) >> 6)];
    // 4-й символ: 6 останніх бітів 3-го байта
    target[3] = b64_alphabet[triplet[2] & 0x3F];
}

void encode_duplet(const unsigned char* duplet, char* target) {
    target[0] = b64_alphabet[(duplet[0] & 0xFC) >> 2];
    target[1] = b64_alphabet[((duplet[0] & 0x03) << 4) | ((duplet[1] & 0xF0) >> 4)];
    target[2] = b64_alphabet[(duplet[1] & 0x0F) << 2];
    target[3] = '=';
}

void encode_symbol(unsigned char symbol, char* target) {
    target[0] = b64_alphabet[(symbol & 0xFC) >> 2];
    target[1] = b64_alphabet[(symbol & 0x03) << 4];
    target[2] = '=';
    target[3] = '=';
}

void encode_file(const string& input_path, const string& output_path, const string& comment = "") {
    ifstream in(input_path, ios::binary);
    ofstream out(output_path);
    if (!in || !out) {
        cerr << "ERROR!!! Source file does not open" << endl;
        return;
    }
    if (!comment.empty()) {
        int pos = 0;
        while (pos < comment.length()) {
            out << "-"; // Початок коментаря
            out << comment.substr(pos, 75) << "\n";
            pos += 75;
        }
    }
    unsigned char buffer[3];
    char target[4];
    int char_count = 0;
    // По 3 байти
    while (in.read(reinterpret_cast<char*>(buffer), 3)) {
        encode_triplet(buffer, target);
        out.write(target, 4);
        char_count += 4;
        // 76 символів => переходимо на новий рядок
        if (char_count >= 76) {
            out << "\n";
            char_count = 0;
        }
    }
    // Залишок <= 1/2 байти
    streamsize remainder = in.gcount();
    if (remainder > 0) {
        if (remainder == 2) {
            encode_duplet(buffer, target);
        }
        else if (remainder == 1) {
            encode_symbol(buffer[0], target);
        }
        out.write(target, 4);
        out << "\n";
    }
}

int get_b64_index(char c) {
    // 'A'-'Z' (0-25)
    if (c >= 'A' && c <= 'Z') return c - 'A';
    // 'a'-'z' (26-51)
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    // '0'-'9' (52-61)
    if (c >= '0' && c <= '9') return c - '0' + 52;
    // Спеціальні символи
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return 0;
    // Помилка
    return -1;
}

int decode_triplet(const char* code, const long long* pos, unsigned char* Buf) {
    int n;
    // 1-й символ => початок 1-го байта
    n = get_b64_index(code[0]);
    if (n == -1) return 1;
    Buf[0] = (n << 2);
    // 2-й символ => кінець 1-го байта і початок 2-го
    n = get_b64_index(code[1]);
    if (n == -1) return 2;
    Buf[0] = Buf[0] | (n >> 4);  // 2 біти в кінець
    Buf[1] = (n & 0x0F) << 4;    // 4 біти в початок
    // 3-й символ => кінець 2-го байта і початок 3-го
    n = get_b64_index(code[2]);
    if (n == -1) return 3;
    Buf[1] = Buf[1] | (n >> 2);  // 4 біти в кінець
    Buf[2] = (n & 0x03) << 6;    // 2 біти в початок
    // 4-й символ => кінець 3-го байта
    n = get_b64_index(code[3]);
    if (n == -1) return 4;
    Buf[2] = Buf[2] | n;         // Останні 6 бітів
    return 0;
}

int decode_duplet(const char* code, const long long* pos, unsigned char* Buf) {
    int n;
    n = get_b64_index(code[0]);
    if (n == -1) return 1;
    Buf[0] = (unsigned char)(n << 2);
    n = get_b64_index(code[1]);
    if (n == -1) return 2;
    Buf[0] = Buf[0] | (unsigned char)(n >> 4);
    Buf[1] = (unsigned char)((n & 0x0F) << 4);
    n = get_b64_index(code[2]);
    if (n == -1) return 3;
    Buf[1] = Buf[1] | (unsigned char)(n >> 2);
    return 0;
}

int decode_symbol(const char* code, const long long* pos, unsigned char* Buf) {
    int n;
    n = get_b64_index(code[0]);
    if (n == -1) return 1;
    Buf[0] = (unsigned char)(n << 2);
    n = get_b64_index(code[1]);
    if (n == -1) return 2;
    Buf[0] = Buf[0] | (unsigned char)(n >> 4);
    return 0;
}

void decode_file(const string& input_path, const string& output_path) {
    ifstream in(input_path);
    ofstream out(output_path, ios::binary);
    if (!in || !out) {
        cerr << "ERROR!!! Could not open files for decoding." << endl;
        return;
    }
    char char_block[4];       // 4 символи Base64
    long long pos_block[4];   // Їх позиції
    int count = 0;            // Заповнення блоку
    long long global_pos = 0; // Глобальна позиція байта
    char c;
    while (in.get(c)) {
        global_pos++;
        if (c == '-') { // Пропуск коментарів
            string com;
            getline(in, com);
            global_pos += com.length();
            continue;
        }
        if (isspace(c)) continue; // Пропуск пробілів, табуляції та переносів
        char_block[count] = c;
        pos_block[count] = global_pos;
        count++;
        if (count == 4) {
            unsigned char out_bytes[3] = { 0, 0, 0 };
            int error_code = 0;
            int bytes_to_write = 0;
            if (char_block[2] == '=') bytes_to_write = 1;      // X==
            else if (char_block[3] == '=') bytes_to_write = 2; // XX=
            else bytes_to_write = 3;                           // ХХХ
            if (bytes_to_write == 1)      error_code = decode_symbol(char_block, pos_block, out_bytes);
            else if (bytes_to_write == 2) error_code = decode_duplet(char_block, pos_block, out_bytes);
            else
                if (error_code > 0) {
                    int idx = error_code - 1; // Індекс помилки
                    cerr << "Error: Invalid character '" << char_block[idx] << "' at position " << pos_block[idx] << " (replaced with *)" << endl;
                    char temp_char = char_block[idx];
                    char_block[idx] = 'A';
                    if (bytes_to_write == 1)      decode_symbol(char_block, pos_block, out_bytes);
                    else if (bytes_to_write == 2) decode_duplet(char_block, pos_block, out_bytes);
                    else                          decode_triplet(char_block, pos_block, out_bytes);
                    if (idx == 0 || idx == 1) out_bytes[0] = '*';
                    else if (idx == 2)        out_bytes[1] = '*';
                    else if (idx == 3)        out_bytes[2] = '*'; decode_triplet(char_block, pos_block, out_bytes);
                }
            out.write(reinterpret_cast<char*>(out_bytes), bytes_to_write);
            count = 0;
        }
    }
    in.close();
    out.close();
}

int main()
{
    int mode, name, comment;
    string in_f, out_f, comment_text = "";
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
            out_f = in_f + ".txt";
        }
    }
    if (mode == 1) {
        cout << "\nDo you want to add a comment to the file?\n1. Yes\n2. No\nChoice: ";
        cin >> comment;
        cin.ignore();
        if (comment == 1) {
            cout << "Enter your comment: ";
            getline(cin, comment_text);
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
