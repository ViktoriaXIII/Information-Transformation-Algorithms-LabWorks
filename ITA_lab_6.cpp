#pragma once
#include <iostream>
#include <algorithm>
#include "LZW_basic.h"
#include "Huffman_basic.h"
#include "Pipeline.h"


using namespace std;

string generate_output_filename(const string& input, CodecType codec, TransformMode mode, bool is_compress) {
    if (is_compress) {
        string ext = "";
        if (mode == BWT_ONLY) ext = ".bwt";
        else if (mode == MTF_ONLY) ext = ".mtf";
        else if (mode == BWT_AND_MTF) ext = ".bwt_mtf";
        string cod = (codec == LZW) ? ".lzw" : ".huff";
        return input + ext + cod;
    }
    else return input + ".dec";
}

int main() {
    int codec_choice, mode_choice, action_choice;
    string input_file;
    while (true) {
        cout << "Options:\n";
        cout << "1. Compress\n";
        cout << "2. Decompress\n";
        cout << "3. Exit\n";
        cout << "Choose an option: ";
        cin >> action_choice;
        if (action_choice == 3) break;
        cout << "\nChoose a basic algorithm:\n1. LZW\n2. Huffman\nEnter a number: ";
        cin >> codec_choice;
        CodecType codec = (codec_choice == 1) ? LZW : HUFFMAN;
        cout << "\nChoose a modification:\n";
        cout << "1. Basic\n";
        cout << "2. BWT\n";
        cout << "3. MTF\n";
        cout << "4. BWT + MTF\n";
        cout << "Enter a number:  ";
        cin >> mode_choice;
        TransformMode mode;
        if (mode_choice == 1) mode = NONE;
        else if (mode_choice == 2) mode = BWT_ONLY;
        else if (mode_choice == 3) mode = MTF_ONLY;
        else mode = BWT_AND_MTF;
        cout << "\nEnter the name of the input file: ";
        cin.ignore();
        getline(cin, input_file);
        string output_file = generate_output_filename(input_file, codec, mode, action_choice == 1);
        if (action_choice == 1) {
            run_compression_pipeline(input_file, output_file, mode, codec);
            cout << "The file was compressed succesfully: " << output_file << "\n";
        }
        else {
            run_decompression_pipeline(input_file, output_file, mode, codec);
            cout << "The file was decompressed succesfully: " << output_file << "\n";
        }
    }
}