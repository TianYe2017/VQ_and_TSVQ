#pragma once
#ifndef QUANTIZATION_H
#define QUANTIZATION_H
#include "stdafx.h"
#include "io.h"
#include "algorithm"
#include "convert_data_brutal.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream> 
#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>


using namespace std;

class Quantization_1D
{
private:
	vector<int> t;
	vector<int> r;
	int pdf[256];
	double current_mse;
	double previous_mse = 100000.0;
	int rate;
	int size_of_sample;
	

	int Compute_ri(int t_start, int t_end);
	double Compute_MSE(void);
	int Find_New_Value(int original);



public:
	Quantization_1D(int rate_in) 
	{
		rate = rate_in;
		for (int i = 0; i < 256; i++) 
		{
			pdf[i] = 0;
		}
		size_of_sample = 0;
	}
	bool halt_sign = false;
	int num_of_iter = 0;
	void Add_Train_Data(vector<unsigned char> input);
	void Init(void);
	void Update_Once(void);
	vector<unsigned char> Reconstruct_Image(vector<unsigned char> input);
	void Computer_PSNR(vector<unsigned char> original, vector<unsigned char> new_file);

};


class Quantization_vector 
{
private:
	int block_size;
	int code_book_size;
	vector<vector<int>> code_book;
	vector<vector<float>> vector_set;
	vector<int> frequency_map;
	vector<unsigned char> Find_Center(vector<unsigned char> v);



public:
	Quantization_vector(int block_size_in, int code_book_size_in) 
	{
		block_size = block_size_in;
		code_book_size = code_book_size_in;
		for (int i = 0; i < code_book_size_in; i++) 
		{
			frequency_map.push_back(0);
		}
	}

	//vector<unsigned char> Encode(vector<unsigned char> input);
	void Add_New_Image(vector<unsigned char> new_image);
	void Build_code_book(void);
	void Quantization_vector::Print_code_book(void);
	vector<unsigned char> Reconstruct_Image(vector<unsigned char> original_image);
	void Computer_PSNR(vector<unsigned char> original, vector<unsigned char> new_file);
	void reset_frequency_map(void);
	vector<int> Get_Frequence_Map(void);
};


struct TSVQ_node 
{
	TSVQ_node* father = NULL;
	TSVQ_node* left_child = NULL;
	TSVQ_node* right_child = NULL;
	int level = -1;
	vector<int> member_index;
	vector<float> center;
	bool is_used = false;

	//build empty node
	TSVQ_node() 
	{
		member_index.clear();
		center.clear();
	}
	
	//build a node
	TSVQ_node(vector<int> index_in, vector<float> center_in, int level_in) 
	{   
		member_index.clear();
		center.clear();
		for (vector<int>::iterator it = index_in.begin(); it != index_in.end(); it++)
		{
			member_index.push_back(*it);
		}
		for (vector<float>::iterator it = center_in.begin(); it != center_in.end(); it++)
		{
			center.push_back(*it);
		}
		level = level_in;
		is_used = true;
	}
};


class Quantization_TSVQ 
{
private:
	int block_size;
	int num_of_bit;
	vector<vector<float>> vector_set;
	vector<TSVQ_node> tree;
	vector<int> frequency_map;
	vector<vector<float>> book;

	float Get_Distance(vector<float> v1, vector<float> v2);
	void K_Means(vector<int> input, int current_level);
	void Create_All_The_Nodes(void);
	void Struct_Tree(void);
	vector<unsigned char> Encode_one_block(vector<unsigned char> block);
	void Quantization_TSVQ::Build_Book(void);
	void Quantization_TSVQ::Find_Index_In_Book(vector<float> target);
	




public:
	Quantization_TSVQ(int block_size_in,int num_of_bit_in) 
	{
		block_size = block_size_in;
		num_of_bit = num_of_bit_in;	
	}

	void Init(void);
	void Add_New_Image(vector<unsigned char> new_image);
	void Build_TSVQ_Tree(void);
	vector<unsigned char> Reconstruct_Image(vector<unsigned char> original_image);
	void Computer_PSNR(vector<unsigned char> original, vector<unsigned char> new_file);
	void reset_frequency_map(void);
	
	vector<int> Get_Frequence_Map(void);


};







#endif 

