#pragma once
#include "BWT_MTF.h"
#include "LZW_basic.h"
#include "Huffman_basic.h"

enum TransformMode { NONE, BWT_ONLY, MTF_ONLY, BWT_AND_MTF };
enum CodecType { LZW, HUFFMAN };

void run_compression_pipeline(const string& inPath, const string& outPath, TransformMode mode, CodecType codec) {
    ifstream in(inPath, ios::binary);
    if (!in) { cout << "Error opening input file\n"; return; }
    string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();
    string temp_file = inPath + ".pipeline.tmp";
    ofstream out_temp(temp_file, ios::binary);
    int primary_index = -1;
    string processed_data = "";
    if (mode == NONE) processed_data = content;
    else if (mode == BWT_ONLY) {
        BWTResult bwt = bwt_encode(content);
        primary_index = bwt.primary_index;
        processed_data = bwt.text;
    }
    else if (mode == MTF_ONLY) {
        processed_data = mtf_encode(content);
    }
    else if (mode == BWT_AND_MTF) {
        BWTResult bwt = bwt_encode(content);
        primary_index = bwt.primary_index;
        processed_data = mtf_encode(bwt.text);
    }
    out_temp.write((char*)&primary_index, sizeof(primary_index));
    out_temp.write(processed_data.data(), processed_data.length());
    out_temp.close();
    if (codec == LZW) compress_LZW(temp_file, outPath);
    else compress_Huff(temp_file, outPath);
    remove(temp_file.c_str());
}

void run_decompression_pipeline(const string& inPath, const string& outPath, TransformMode mode, CodecType codec) {
    string temp_file = inPath + ".pipeline.dec.tmp";
    if (codec == LZW) decompress_LZW(inPath, temp_file);
    else decompress_Huff(inPath, temp_file);
    ifstream in_temp(temp_file, ios::binary);
    if (!in_temp) { cout << "Pipeline error during decompression\n"; return; }
    int primary_index;
    in_temp.read((char*)&primary_index, sizeof(primary_index));
    string packed_data((istreambuf_iterator<char>(in_temp)), istreambuf_iterator<char>());
    in_temp.close();
    remove(temp_file.c_str());
    string restored_data = "";
    if (mode == NONE) restored_data = packed_data;
    else if (mode == BWT_ONLY) {
        restored_data = bwt_decode(packed_data, primary_index);
    }
    else if (mode == MTF_ONLY) {
        restored_data = mtf_decode(packed_data);
    }
    else if (mode == BWT_AND_MTF) {
        string mtf_decoded = mtf_decode(packed_data);
        restored_data = bwt_decode(mtf_decoded, primary_index);
    }
    ofstream out_final(outPath, ios::binary);
    out_final.write(restored_data.data(), restored_data.length());
    out_final.close();
}