#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class BitStream {
private:
    fstream file;
    uint8_t buffer; // Тимчасовий байт для накопичення бітів
    int curPos; // Поточна позиція у buffer
    bool isWriting;
    void fillBuffer() {
        if (curPos > 0) {
            file.put(buffer);
            buffer = 0;
            curPos = 0;
        }
    }
public:
    BitStream(const string& filename, ios::openmode mode) : buffer(0), curPos(0) {
        file.open(filename, mode | ios::binary);
        isWriting = (mode & ios::out);
    }
    ~BitStream() {
        if (isWriting) fillBuffer(); // Вирівнювання '0-ми при закритті
        if (file.is_open()) file.close();
    }
    void WriteBitSequence(const vector<uint8_t>& data, int bitCount) {
        for (int i = 0; i < bitCount; i++) {
            int byteIdx = i / 8;
            int bitIdx = i % 8;
            bool bit = (data[byteIdx] >> bitIdx) & 1;
            if (bit) buffer = buffer | (1 << curPos);
            curPos++;
            if (curPos == 8) {
                file.put(buffer);
                buffer = 0;
                curPos = 0;
            }
        }
    }
    void ReadBitSequence(vector<uint8_t>& data, int bitCount) {
        data.clear();
        data.resize((bitCount + 7) / 8, 0);
        for (int i = 0; i < bitCount; i++) {
            if (curPos == 0) {
                if (!file.get(reinterpret_cast<char&>(buffer))) return;
            }
            bool bit = (buffer >> curPos) & 1;
            if (bit) {
                int byteIdx = i / 8;
                int bitIdx = i % 8;
                data[byteIdx] = data[byteIdx] | (1 << bitIdx);
            }
            curPos++;
            if (curPos == 8) curPos = 0;
        }
    }
};

int main() {
    // Запис
    {
        /*BitStream bs("test.bin", ios::out);
        vector<uint8_t> a1 = { 0xE1, 0x01 };
        vector<uint8_t> a2 = { 0xEE, 0x00 };
        bs.WriteBitSequence(a1, 9);
        bs.WriteBitSequence(a2, 9);*/
        BitStream bs("test.bin", ios::out | ios::trunc);
        vector<uint8_t> s1 = { 0x1B }; // 11011
        vector<uint8_t> s2 = { 0xF0, 0x0F }; // 11110000 1111
        vector<uint8_t> s3 = { 0x05 }; // 101
        bs.WriteBitSequence(s1, 5);
        bs.WriteBitSequence(s2, 12);
        bs.WriteBitSequence(s3, 3);
    }
    // Зчитування
    {
        /*BitStream bs("test.bin", ios::in);
        vector<uint8_t> b1, b2;
        bs.ReadBitSequence(b1, 11);
        bs.ReadBitSequence(b2, 7);
        // Перевірка
        auto printHex = [](const string& name, const vector<uint8_t>& d) {
            cout << name << ": ";
            for (auto b : d) printf("%02X ", b);
            cout << endl;
            };
        printHex("b1", b1);
        printHex("b2", b2);*/
        BitStream bs("test.bin", ios::in);
        vector<uint8_t> r1, r2, r3;
        bs.ReadBitSequence(r1, 5);
        bs.ReadBitSequence(r2, 12);
        bs.ReadBitSequence(r3, 3);
        cout << "S1:  ";
        for (auto b : r1) printf("%02X ", b);
        cout << endl;
        cout << "S2: ";
        for (auto b : r2) printf("%02X ", b);
        cout << endl;
        cout << "S3:  ";
        for (auto b : r3) printf("%02X ", b);
        cout << "\n" << endl;
    }
}
