#pragma once
#ifndef IO_H
#define IO_H
#include "stdafx.h"
#include "convert_data_brutal.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream> 

using namespace std;

vector<unsigned char> ReadBinaryFile(string File_path);
vector<bool> ReadBinaryFileToBool(string File_path);
vector<unsigned char> Read_Binary_File_To_Unsigned_Char_Symbol(string File_path, int n);
vector<unsigned char> Read_Binary_File_To_Unsigned_Char(string File_path);
void Write_String_To_Dat(string input, string path);
unsigned char Bool_To_Unsigned_Char(bool in);
vector<bool> ReadBinaryFileToBoolWithoutBitMap(string File_path);
void Write_Image_To_Dat(vector<unsigned char> input, string path);


#endif // !IO_H