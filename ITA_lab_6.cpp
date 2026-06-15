#pragma once
#include <iostream>
#include <algorithm>
#include "LZW_basic.h"
#include "Huffman_basic.h"

using namespace std;

// BWT
struct BWTResult {
    string text;
    int primary_index;
};

BWTResult bwt_encode(const string& input) {
    int n = input.length();
    if (n == 0) return { "", 0 };
    vector<int> shifts(n);
    for (int i = 0; i < n; i++) shifts[i] = i;
    sort(shifts.begin(), shifts.end(), [&input, n](int a, int b) {
        for (int i = 0; i < n; i++) {
            char charA = input[(a + i) % n];
            char charB = input[(b + i) % n];
            if (charA != charB) return charA < charB;
        }
        return false;
        });
    string last_column = "";
    int primary_index = 0;
    for (int i = 0; i < n; i++) {
        last_column += input[(shifts[i] + n - 1) % n];
        if (shifts[i] == 0) primary_index = i;
    }
    return { last_column, primary_index };
}

string bwt_decode(const string& bwt_text, int primary_index) {
    int n = bwt_text.length();
    if (n == 0) return "";
    string first_column = bwt_text;
    sort(first_column.begin(), first_column.end());
    vector<int> count(256, 0);
    vector<int> next_row(n);
    vector<int> first_pos(256, -1);
    for (int i = 0; i < n; i++) {
        unsigned char c = first_column[i];
        if (first_pos[c] == -1) first_pos[c] = i;
    }
    for (int i = 0; i < n; i++) {
        unsigned char c = bwt_text[i];
        next_row[first_pos[c] + count[c]] = i;
        count[c]++;
    }
    int curr = primary_index;
    vector<char> ans(n);
    for (int i = n - 1; i >= 0; i--) {
        ans[i] = bwt_text[curr];
        curr = next_row[curr];
    }
    return string(ans.begin(), ans.end());
}

// MTF
string mtf_encode(const string& input) {
    vector<uint8_t> playlist(256);
    for (int i = 0; i < 256; i++) playlist[i] = i;
    string output = "";
    output.reserve(input.length());
    for (char c : input) {
        uint8_t target = (uint8_t)c;
        int idx = 0;
        while (playlist[idx] != target) idx++;
        output += (char)((uint8_t)idx);
        for (int i = idx; i > 0; i--) playlist[i] = playlist[i - 1];
        playlist[0] = target;
    }
    return output;
}

string mtf_decode(const string& input) {
    vector<uint8_t> playlist(256);
    for (int i = 0; i < 256; i++) playlist[i] = i;
    string output = "";
    output.reserve(input.length());
    for (char c : input) {
        int idx = (uint8_t)c;
        uint8_t target = playlist[idx];
        output += (char)target;
        for (int i = idx; i > 0; i--) playlist[i] = playlist[i - 1];
        playlist[0] = target;
    }
    return output;
}

int main()
{
    cout << "Hello World!\n";
}