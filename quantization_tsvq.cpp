#include "stdafx.h"
#include "quantization.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream>

using namespace std;
using namespace cv;

void Quantization_TSVQ::Build_Book(void)
{
	for (int i = 0; i < tree.size(); i++) 
	{
		if (tree[i].level == num_of_bit) 
		{
			book.push_back(tree[i].center);
		}
	}

	cout << "size_of_book: "<< book.size() << endl;
}

void Quantization_TSVQ::Find_Index_In_Book(vector<float> target) 
{
	for (int i = 0; i < book.size(); i++) 
	{
		if (target == book[i]) 
		{
			frequency_map[i] ++;
			break;
		}
	}
}

void Quantization_TSVQ::Init(void)
{
    //生成一个root节点
	TSVQ_node root = TSVQ_node();
	root.is_used = true;
	root.level = 0;
	for (int i = 0; i < vector_set.size(); i++)
	{
		root.member_index.push_back(i);
	}
	tree.push_back(root);
}


void Quantization_TSVQ::Add_New_Image(vector<unsigned char> new_image)
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


float Quantization_TSVQ::Get_Distance(vector<float> v1, vector<float> v2)
{
	float d = 0.0;
	for (int i = 0; i < v1.size(); i++) 
	{
		d += pow((v1[i] - v2[i]), 2);
	}
	d = sqrt(d);
	return d;
}


void Quantization_TSVQ::K_Means(vector<int> input,int current_level)
{
	if (current_level < num_of_bit + 1) 
	{
		vector<int> cluster_1;
		vector<int> cluster_2;
		vector<float> center_point1;
		vector<float> center_point2;
		//step1 重构输入集
		vector<vector<float>> target;
		for (vector<int>::iterator it = input.begin(); it != input.end(); it++)
		{
			target.push_back(vector_set[*it]);
		}
		//step2 转换为MAT
		Mat M = Mat(target.size(), pow(block_size, 2), CV_32F);
		for (int i = 0; i < target.size(); i++)
		{
			for (int j = 0; j < pow(block_size, 2); j++)
			{
				M.at<float>(i, j) = target[i][j];
			}
		}
		// step3 K Means
		cout << "K-Means ing..." << endl;
		Mat CenterPoint, labels;
		kmeans(M, 2, labels, TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 300, 1), 3, KMEANS_PP_CENTERS, CenterPoint);
		//kmeans(M, 2, labels, TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 300, 0), 3, KMEANS_RANDOM_CENTERS, CenterPoint);
		// step4 输出center point
		for (int i = 0; i < pow(block_size, 2); i++)
		{
			center_point1.push_back(CenterPoint.at<float>(0, i));
		}
		for (int i = 0; i < pow(block_size, 2); i++)
		{
			center_point2.push_back(CenterPoint.at<float>(1, i));
		}
		// step5 分配数据
		for (int i = 0; i < input.size(); i++)
		{
			vector<float> temp;
			for (int j = 0; j < pow(block_size, 2); j++)
			{
				temp.push_back(vector_set[input[i]][j]);
			}
			if (Get_Distance(temp, center_point1) < Get_Distance(temp, center_point2))
			{
				cluster_1.push_back(input[i]);
			}
			else
			{
				cluster_2.push_back(input[i]);
			}
		}
		// step6 生成节点
		TSVQ_node node1 = TSVQ_node(cluster_1, center_point1, current_level);
		TSVQ_node node2 = TSVQ_node(cluster_2, center_point2, current_level);
		tree.push_back(node1);
		tree.push_back(node2);
		// 递归调用自己
		K_Means(cluster_1, current_level + 1);
		K_Means(cluster_2, current_level + 1);
	}
}


void Quantization_TSVQ::Create_All_The_Nodes(void) 
{
	vector<int> start;
	for (int i = 0; i < vector_set.size(); i++) 
	{
		start.push_back(i);
	}
	K_Means(start, 1);
}

bool Is_Parent(TSVQ_node kid, TSVQ_node parent) 
{   
	bool out = false;
	if (kid.level - 1 == parent.level)
	{   
		//cout << "hit" << endl;
		int sample = kid.member_index[0];
		//cout << sample << endl;
		for (int i = 0; i < parent.member_index.size(); i++)
		{
			if (sample == parent.member_index[i])
			{
				out = true;
				break;
			}
		}
	}
	//cout << out << endl;
	return out;
}

void Quantization_TSVQ::Struct_Tree(void) 
{   
	for (int i = 1; i < tree.size(); i++) 
	{
		for (int j = 0; j < tree.size(); j++) 
		{
			if (Is_Parent(tree[i], tree[j]) == true) 
			{
				tree[i].father = &tree[j];
				if (tree[j].left_child == NULL) 
				{
					tree[j].left_child = &tree[i];
				}
				else 
				{
					tree[j].right_child = &tree[i];
				}
				break;
			}//find parent
		}
	}//find parent for node
}

void Quantization_TSVQ:: Build_TSVQ_Tree(void)
{
	Create_All_The_Nodes();
	Build_Book();
	Struct_Tree();

	cout << "Size of tree is:  " << tree.size() << endl;
	
	for (int i = 1; i < tree.size(); i++) 
	{
		if (tree[i].level == num_of_bit) 
		{
			for (int j = 0; j < pow(block_size, 2); j++) 
			{
				cout << (int)tree[i].center[j] << " ";
			}
			cout << " " << endl;
		}	
	}
}

vector<unsigned char> Quantization_TSVQ::Encode_one_block(vector<unsigned char> block)
{   
	vector<float> current;
	for (int i = 0; i < pow(block_size, 2); i++) 
	{
		current.push_back((float)U8_to_0_255(block[i]));
	}
	TSVQ_node *p = &tree[0];
	while (p->level != num_of_bit) 
	{    
		vector<float> left = p->left_child->center;
		/*for (int i = 0; i < pow(block_size, 2); i++) 
		{
			left.push_back(p->left_child->center[i]);
		}*/
		vector<float> right = p->right_child->center;
		/*for (int i = 0; i < pow(block_size, 2); i++)
		{
			right.push_back(p->right_child->center[i]);
		}*/
		if (Get_Distance(left, current) < Get_Distance(right, current)) 
		{
			p = p->left_child;
		}
		else 
		{
			p = p->right_child;
		}
	}
	vector<float> goal = p->center;
	Find_Index_In_Book(goal);
	vector<unsigned char> output;
	for (int i = 0; i < pow(block_size, 2); i++)
	{
		output.push_back(Zero_255_to_U8((int)goal[i]));
	}
	return output;
}

vector<unsigned char> Quantization_TSVQ::Reconstruct_Image(vector<unsigned char> original_image)
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
			vector<unsigned char> new_block = Encode_one_block(temp);
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

void Quantization_TSVQ::Computer_PSNR(vector<unsigned char> original, vector<unsigned char> new_file)
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




void  Quantization_TSVQ::reset_frequency_map(void)
{
	frequency_map.clear();
	for (int i = 0; i < (int)pow(2,num_of_bit); i++)
	{
		frequency_map.push_back(0);
	}
}

vector<int>  Quantization_TSVQ::Get_Frequence_Map(void)
{
	return frequency_map;
}





