#include "stdafx.h"
#include "quantization.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream>

using namespace std;
using namespace cv;


vector<unsigned char> Quantization_vector::Find_Center(vector<unsigned char> v) 
{
	vector<unsigned char> target;
	int index = 0;
	double Nearset_Distance = 999999999999;
	for (int i = 0; i < code_book.size(); i++) 
	{
	    double L2D = 0;
		for (int j = 0; j < pow(block_size, 2); j++) 
		{
			L2D += (U8_to_0_255(v[j]) - code_book[i][j]) * (U8_to_0_255(v[j]) - code_book[i][j]);
		}
		L2D = sqrt(L2D);
		if (L2D < Nearset_Distance) 
		{   
			index = i;
			target.clear();
			Nearset_Distance = L2D;
			for (int p = 0; p < pow(block_size, 2); p++) 
			{
				target.push_back(Zero_255_to_U8(code_book[i][p]));
			}
		} 
	}//all book
	frequency_map[index]++;
	return target;
}



void Quantization_vector::Add_New_Image(vector<unsigned char> new_image) 
{    
	int size = block_size;
	vector<vector<unsigned char>> matrix;
	//初始化
	for (int y = 0; y < 256; y++)
	{
		vector<unsigned char> temp;
		for (int x = 0; x < 256; x++)
		{
			temp.push_back(0x00);
		}
		matrix.push_back(temp);
	}
	//导入
	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			matrix[y][x] = new_image[256 * y + x];
		}
	}
	//分割
	int it = 256 / size;
	for (int y = 0; y < it; y++)
	{
		for (int x = 0; x < it; x++)
		{   
			vector<float> temp;
			for (int i = 0; i < size; i++)
			{   
				for (int j = 0; j < size; j++)
				{   
					temp.push_back((float)(U8_to_0_255(matrix[y*size + i][x*size + j])));
				}
			}//block complete
			//cout << "size_of_block:  " << temp.size() << endl;
			vector_set.push_back(temp);
		}
	}//image
}


void Quantization_vector::Build_code_book(void) 
{
	Mat M = Mat(vector_set.size(), pow(block_size,2), CV_32F);
	for (int i = 0; i < vector_set.size(); i++)
	{
		for (int j = 0; j < pow(block_size, 2); j++)
		{
			M.at<float>(i, j) = vector_set[i][j];
		}
	}
	cout << "K-Means ing..." << endl;
	Mat CenterPoint, labels;
	kmeans(M, code_book_size, labels, TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 300, 1), 3, KMEANS_PP_CENTERS, CenterPoint);
	cout << "Construct code book ing..." << endl;
	for (int i = 0; i < code_book_size; i++) 
	{
		vector<int> center;
		for (int j = 0; j < pow(block_size, 2); j++) 
		{
			center.push_back((int)CenterPoint.at<float>(i, j));
		}
		code_book.push_back(center);
	}
}

void Quantization_vector::Print_code_book(void) 
{
	for (int i = 0; i < code_book.size(); i++) 
	{
		for (int j = 0; j < pow(block_size, 2); j++) 
		{
			cout << code_book[i][j] << " ";
		}
		cout << " " << endl;
	}
}

vector<unsigned char> Quantization_vector ::Reconstruct_Image(vector<unsigned char> original_image) 
{
	int size = block_size;
	vector<vector<unsigned char>> matrix;
	vector<vector<unsigned char>> matrix_out;
	//初始化
	for (int y = 0; y < 256; y++)
	{
		vector<unsigned char> temp;
		for (int x = 0; x < 256; x++)
		{
			temp.push_back(0x00);
		}
		matrix.push_back(temp);
		matrix_out.push_back(temp);
	}
	//导入
	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			matrix[y][x] = original_image[256 * y + x];
		}
	}
	//分割
	int it = 256 / size;
	for (int y = 0; y < it; y++)
	{
		for (int x = 0; x < it; x++)
		{
			vector<unsigned char> temp;
			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					temp.push_back(matrix[y*size + i][x*size + j]);
				}
			}//block complete
			 //cout << "size_of_block:  " << temp.size() << endl;
			vector<unsigned char> new_block = Find_Center(temp);
			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					matrix_out[y*size + i][x*size + j] = new_block[i * size + j];
				}
			}//block complete		
		}
	}//image
	//重新化成一个列
	vector<unsigned char> output;
	for (int i = 0; i < 256; i++) 
	{
		for (int j = 0; j < 256; j++) 
		{
			output.push_back(matrix_out[i][j]);
		}
	}
	return output;
}

void Quantization_vector::Computer_PSNR(vector<unsigned char> original, vector<unsigned char> new_file)
{
	double out = 0;
	for (int i = 0; i < original.size(); i++)
	{
		out += pow((U8_to_0_255(original[i]) - U8_to_0_255(new_file[i])), 2);
	}
	out = out / original.size();
	cout << "MSE:  " << out << endl;
	out = 10.0 * log10(255.0*255.0 / out);
	cout << "PSNR:  " << out << endl;
}

void Quantization_vector::reset_frequency_map(void) 
{
	frequency_map.clear();
	for (int i = 0; i < code_book_size; i++) 
	{
		frequency_map.push_back(0);
	}
}

vector<int> Quantization_vector:: Get_Frequence_Map(void)
{
	return frequency_map;
}