#include "stdafx.h"
#include "io.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream> 


using namespace std;

// data must smaller than 10MB
vector<unsigned char> ReadBinaryFile(string File_path) 
{   
	vector<unsigned char> output;
	unsigned char buffer[9999999];
	//find the length of the file.
	ifstream in;           //ifstream读文件（ofstream写文件）  
	int length;
	in.open(File_path, ios_base::binary);      // open input file  
	in.seekg(0, std::ios::end);    // go to the end  
	length = in.tellg();           // report location (this is the length)  
	cout << length << endl;
	in.seekg(0, std::ios::beg);    // go back to the beginning  
	in.close();
	//read the data to vector
	FILE *fp_read = fopen(File_path.c_str(), "rb");
	if (fp_read == NULL)
	{
		printf("\nFail to find the file.");
	}
	fread(buffer, sizeof(unsigned char), length, fp_read);
	fclose(fp_read);
	for (int i = 0; i < length; i++) 
	{   
		//cout << (int)buffer[i] << endl;
		output.push_back(buffer[i]);
	}
	cout << "File size (Byte) is: " << length << endl;
	return output;
}


int String_To_Integer_0_1(string zero_or_one) 
{
	if (zero_or_one == "0") 
	{
		return 0;
	}
	else  
	{
		return 1;
	}
}


void Write_String_To_Dat(string input,string path) 
{   
	unsigned char buffer[99999999];
	int index = 0;
	int dat_length = input.size() / 8;
	if (dat_length * 8 != input.size()) 
	{
		int c = (dat_length + 1) * 8 - input.size();
		for (int i = 0; i < c; i++) 
		{
			input += "0";
		}
		dat_length++;
	}
	for (int i = 0; i < dat_length; i++) 
	{   
		int dice[8];
		for (int j = 0 + 8 * i; j < 8 + 8 * i; j++) 
		{
			dice[j - 8 * i] = String_To_Integer_0_1(input.substr(j,1));
		}
		int temp = 128 * dice[0] + 64 * dice[1] + 32 * dice[2] + 16 * dice[3] + 8 * dice[4] + 4 * dice[5] + 2 * dice[6] + dice[7];
		buffer[index] = Zero_255_to_U8(temp);
		index++;
	}
	FILE *fp_write = fopen(path.c_str(), "wb");
	if (fp_write == NULL)
	{
		printf("\nFail to create the file.");
	}
	else
	{
		printf("\nScuess to create the file.");
	}
	fwrite(buffer, sizeof(unsigned char), dat_length, fp_write);
	fclose(fp_write);
	cout << "done: " << path << endl;
}


// data must smaller than 10MB
vector<bool> ReadBinaryFileToBool(string File_path)
{
	vector<unsigned char> output;
	vector<bool> output_bool;
	unsigned char buffer[9999999];
	//find the length of the file.
	ifstream in;           //ifstream读文件（ofstream写文件）  
	int length;
	in.open(File_path, ios_base::binary);      // open input file  
	in.seekg(0, std::ios::end);    // go to the end  
	length = in.tellg();           // report location (this is the length)  
	in.seekg(0, std::ios::beg);    // go back to the beginning  
	in.close();
	//read the data to vector
	FILE *fp_read = fopen(File_path.c_str(), "rb");
	if (fp_read == NULL)
	{
		printf("\nFail to find the file.");
	}
	fread(buffer, sizeof(unsigned char), length, fp_read);
	fclose(fp_read);
	for (int i = 0; i < length; i++)
	{
		//cout << (int)buffer[i] << endl;
		output.push_back(buffer[i]);
	}
	cout << "File size (Byte) is: " << length << endl;
	
	//bit-map

	for (int i = 7; i >= 0; i--)
	{
		for (vector<unsigned char>::iterator it = output.begin(); it != output.end(); it++)
		{
			unsigned char temp = *it;
			output_bool.push_back((bool)((temp >> i) & 1));
		}
	}
	return output_bool;
}


unsigned char Bool_To_Unsigned_Char(bool in) 
{
	if (in == true) 
	{
		return 0x01;
	}
	else
	{
		return 0x00;
	}
}


vector<unsigned char> Read_Binary_File_To_Unsigned_Char_Symbol(string File_path,int n)
{
	vector<unsigned char> output;
	vector<unsigned char> empty;
	unsigned char buffer[9999999];
	//find the length of the file.
	ifstream in;           //ifstream读文件（ofstream写文件）  
	int length;
	in.open(File_path, ios_base::binary);      // open input file  
	in.seekg(0, std::ios::end);    // go to the end  
	length = in.tellg();           // report location (this is the length)  
	in.seekg(0, std::ios::beg);    // go back to the beginning  
	in.close();
	//read the data to vector
	FILE *fp_read = fopen(File_path.c_str(), "rb");
	if (fp_read == NULL)
	{
		printf("\nFail to find the file.");
	}
	fread(buffer, sizeof(unsigned char), length, fp_read);
	fclose(fp_read);
	for (int i = 0; i < length; i++)
	{
		//cout << (int)buffer[i] << endl;
		output.push_back(buffer[i]);
	}
	cout << "File size (Byte) is: " << length << endl;

	//bit-map
	vector<unsigned char> raw;
	for (int i = 7; i >= 0; i--)
	{
		for (vector<unsigned char>::iterator it = output.begin(); it != output.end(); it++)
		{
			unsigned char temp = *it;
			raw.push_back(Bool_To_Unsigned_Char((bool)((temp >> i) & 1)));
		}
	}
	//context-based
	if (n == 1) 
	{
		raw.insert(raw.begin(), 0x00);
		vector<unsigned char> temp0;
		vector<unsigned char> temp1;
		for (int i = 0; i < raw.size() - 1; i++) 
		{
			if (raw[i] == 0x00) 
			{
				if (raw[i + 1] == 0x00) 
				{
					temp0.push_back(0x00);
				}
				else 
				{
					temp0.push_back(0x01);
				}
			}
			else 
			{
				if (raw[i + 1] == 0x00)
				{
					temp1.push_back(0x00);
				}
				else
				{
					temp1.push_back(0x01);
				}
			}		
		}//for
		for (vector<unsigned char>::iterator it = temp1.begin(); it != temp1.end(); it++)
		{
			temp0.push_back(*it);
		}
		return temp0;
	}//end of N=1
	else if (n == 2) 
	{
		raw.insert(raw.begin(), 0x00);
		raw.insert(raw.begin(), 0x00);
		vector<unsigned char> temp00;
		vector<unsigned char> temp01;
		vector<unsigned char> temp10;
		vector<unsigned char> temp11;
		for (int i = 0; i < raw.size() - 2; i++) 
		{
			if (raw[i] == 0x00 && raw[i + 1] == 0x00) 
			{
				if (raw[i + 2] == 0x00) 
				{
					temp00.push_back(0x00);
				}
				else 
				{
					temp00.push_back(0x01);
				}
			}
			else if (raw[i] == 0x00 && raw[i + 1] == 0x01) 
			{
				if (raw[i + 2] == 0x00)
				{
					temp01.push_back(0x00);
				}
				else
				{
					temp01.push_back(0x01);
				}
			}
			else if (raw[i] == 0x11 && raw[i + 1] == 0x10) 
			{
				if (raw[i + 2] == 0x00)
				{
					temp10.push_back(0x00);
				}
				else
				{
					temp10.push_back(0x01);
				}
			}
			else 
			{
				if (raw[i + 2] == 0x00)
				{
					temp11.push_back(0x00);
				}
				else
				{
					temp11.push_back(0x01);
				}
			}		
		}//for
		for (vector<unsigned char>::iterator it = temp01.begin(); it != temp01.end(); it++)
		{
			temp00.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp10.begin(); it != temp10.end(); it++)
		{
			temp00.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp11.begin(); it != temp11.end(); it++)
		{
			temp00.push_back(*it);
		}
		return temp00;
	}//end of N=2
	else if (n == 3) 
	{
		raw.insert(raw.begin(), 0x00);
		raw.insert(raw.begin(), 0x00);
		raw.insert(raw.begin(), 0x00);
		vector<unsigned char> temp000;
		vector<unsigned char> temp001;
		vector<unsigned char> temp010;
		vector<unsigned char> temp011;
		vector<unsigned char> temp100;
		vector<unsigned char> temp101;
		vector<unsigned char> temp110;
		vector<unsigned char> temp111;
		for (int i = 0; i < raw.size() - 3; i++) 
		{
			if (raw[i] == 0x00 && raw[i + 1] == 0x00 && raw[i + 2] == 0x00) 
			{
				if (raw[i + 3] == 0x00)
				{
					temp000.push_back(0x00);
				}
				else
				{
					temp000.push_back(0x01);
				}
			}
			else if (raw[i] == 0x00 && raw[i + 1] == 0x00 && raw[i + 2] == 0x01) 
			{
				if (raw[i + 3] == 0x00)
				{
					temp001.push_back(0x00);
				}
				else
				{
					temp001.push_back(0x01);
				}
			}
			else if (raw[i] == 0x00 && raw[i + 1] == 0x01 && raw[i + 2] == 0x00)
			{
				if (raw[i + 3] == 0x00)
				{
					temp010.push_back(0x00);
				}
				else
				{
					temp010.push_back(0x01);
				}
			}
			else if (raw[i] == 0x00 && raw[i + 1] == 0x01 && raw[i + 2] == 0x01)
			{
				if (raw[i + 3] == 0x00)
				{
					temp011.push_back(0x00);
				}
				else
				{
					temp011.push_back(0x01);
				}
			}
			else if (raw[i] == 0x01 && raw[i + 1] == 0x00 && raw[i + 2] == 0x00)
			{
				if (raw[i + 3] == 0x00)
				{
					temp100.push_back(0x00);
				}
				else
				{
					temp100.push_back(0x01);
				}
			}
			else if (raw[i] == 0x01 && raw[i + 1] == 0x00 && raw[i + 2] == 0x01)
			{
				if (raw[i + 3] == 0x00)
				{
					temp101.push_back(0x00);
				}
				else
				{
					temp101.push_back(0x01);
				}
			}
			else if (raw[i] == 0x01 && raw[i + 1] == 0x01 && raw[i + 2] == 0x00)
			{
				if (raw[i + 3] == 0x00)
				{
					temp110.push_back(0x00);
				}
				else
				{
					temp110.push_back(0x01);
				}
			}
			else
			{
				if (raw[i + 3] == 0x00)
				{
					temp111.push_back(0x00);
				}
				else
				{
					temp111.push_back(0x01);
				}
			}	
		}//for
		for (vector<unsigned char>::iterator it = temp001.begin(); it != temp001.end(); it++)
		{
			temp000.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp010.begin(); it != temp010.end(); it++)
		{
			temp000.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp011.begin(); it != temp011.end(); it++)
		{
			temp000.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp100.begin(); it != temp100.end(); it++)
		{
			temp000.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp101.begin(); it != temp101.end(); it++)
		{
			temp000.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp110.begin(); it != temp110.end(); it++)
		{
			temp000.push_back(*it);
		}
		for (vector<unsigned char>::iterator it = temp111.begin(); it != temp111.end(); it++)
		{
			temp000.push_back(*it);
		}
		return temp000;
	}
	else 
	{
		cout << "Invalid N." << endl;
		return empty;
	}
}


vector<unsigned char> Read_Binary_File_To_Unsigned_Char(string File_path)
{
	vector<unsigned char> output;
	vector<unsigned char> empty;
	unsigned char buffer[9999999];
	//find the length of the file.
	ifstream in;           //ifstream读文件（ofstream写文件）  
	int length;
	in.open(File_path, ios_base::binary);      // open input file  
	in.seekg(0, std::ios::end);    // go to the end  
	length = in.tellg();           // report location (this is the length)  
	in.seekg(0, std::ios::beg);    // go back to the beginning  
	in.close();
	//read the data to vector
	FILE *fp_read = fopen(File_path.c_str(), "rb");
	if (fp_read == NULL)
	{
		printf("\nFail to find the file.");
	}
	fread(buffer, sizeof(unsigned char), length, fp_read);
	fclose(fp_read);
	for (int i = 0; i < length; i++)
	{
		//cout << (int)buffer[i] << endl;
		output.push_back(buffer[i]);
	}
	cout << "File size (Byte) is: " << length << endl;

	//bit-map
	vector<unsigned char> raw;
	for (int i = 7; i >= 0; i--)
	{
		for (vector<unsigned char>::iterator it = output.begin(); it != output.end(); it++)
		{
			unsigned char temp = *it;
			raw.push_back(Bool_To_Unsigned_Char((bool)((temp >> i) & 1)));
		}
	}
	return raw;
}


vector<bool> ReadBinaryFileToBoolWithoutBitMap(string File_path)
{
	vector<unsigned char> output;
	vector<bool> output_bool;
	unsigned char buffer[9999999];
	//find the length of the file.
	ifstream in;           //ifstream读文件（ofstream写文件）  
	int length;
	in.open(File_path, ios_base::binary);      // open input file  
	in.seekg(0, std::ios::end);    // go to the end  
	length = in.tellg();           // report location (this is the length)  
	in.seekg(0, std::ios::beg);    // go back to the beginning  
	in.close();
	//read the data to vector
	FILE *fp_read = fopen(File_path.c_str(), "rb");
	if (fp_read == NULL)
	{
		printf("\nFail to find the file.");
	}
	fread(buffer, sizeof(unsigned char), length, fp_read);
	fclose(fp_read);
	for (int i = 0; i < length; i++)
	{
		//cout << (int)buffer[i] << endl;
		output.push_back(buffer[i]);
	}
	cout << "File size (Byte) is: " << length << endl;

		for (vector<unsigned char>::iterator it = output.begin(); it != output.end(); it++)
		{   
			unsigned char temp = *it;
			for (int i = 7; i >= 0; i--)
			{			
				output_bool.push_back((bool)((temp >> i) & 1));
			}
		}
	return output_bool;
}


void Write_Image_To_Dat(vector<unsigned char> input,string path) 
{
	unsigned char buffer[99999999];
	int index = 0;
	for (int i = 0; i < input.size(); i++) 
	{
		buffer[index] = input[i];
		index ++;
	}
	FILE *fp_write = fopen(path.c_str(), "wb");
	if (fp_write == NULL)
	{
		printf("\nFail to create the file.");
	}
	else
	{
		printf("\nScuess to create the file.");
	}
	fwrite(buffer, sizeof(unsigned char), input.size(), fp_write);
	fclose(fp_write);
	cout << "done: " << path << endl;

}