#include "stdafx.h"
#include "quantization.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream>

using namespace std;

int Quantization_1D::Find_New_Value(int original)
{   
	int output = -1;
	for (int i = 0; i < pow(2, rate); i++) 
	{
		if (original != 255) 
		{
			if (original >= t[i] && original < t[i + 1]) 
			{
				output = r[i];
				break;
			}
		}
		else 
		{
			output = r[pow(2, rate) - 1];
		}
	
	}
	if (output == -1) 
	{
		cout << "FATAL ERROR: Can not find correspond R." << endl;
		while (1);
	}
	return output;
}


double Quantization_1D::Compute_MSE(void) 
{   
	double MSE = 0;
	for (int i = 0; i < 256; i++) 
	{
		double temp = pow((i - Find_New_Value(i)), 2);
		temp = temp * (double)pdf[i];
		MSE += temp;
	}
	MSE = MSE / size_of_sample;
	return MSE;
}

void Quantization_1D::Add_Train_Data(vector<unsigned char> input) 
{
	for (vector<unsigned char>::iterator it = input.begin(); it != input.end(); it++)
	{
		pdf[U8_to_0_255(*it)] ++;
	}
	size_of_sample += input.size();
}


int Quantization_1D::Compute_ri(int t_start, int t_end) 
{
	int C1 = 0;
	int C2 = 0;
	for (int i = t_start; i <= t_end; i++) 
	{
		C1 += i * pdf[i];
		C2 += pdf[i];
	}
	return C1 / C2;
}

void Quantization_1D::Init(void)
{
	int num_interval = pow(2,rate);
	int interval_length = 256 / num_interval;
	t.push_back(0);
	for (int i = 1; i < num_interval; i++) 
    {
		t.push_back(i * interval_length - 1);
	}
	t.push_back(255);
	cout << "t_size:  " << t.size() << endl; 
	for (int i = 0; i < num_interval; i++)
	{
		r.push_back(Compute_ri(t[i], t[i + 1]));
	}
	cout << "r_size:  " << r.size() << endl;

}

void Quantization_1D::Update_Once(void) 
{   

	//update ti
	for (int i = 1; i < t.size() - 1; i++) 
	{
		t[i] = (r[i] + r[i - 1]) / 2;
	}
	cout << "T: ";
	for (vector<int>::iterator it = t.begin(); it != t.end(); it++)
	{
		cout << *it << " ";
	}
	cout << " " << endl;
	//update ri
	for (int i = 0; i < pow(2, rate); i++)
	{
		r[i] = Compute_ri(t[i], t[i + 1]);
	}
	cout << "R: ";
	for (vector<int>::iterator it = r.begin(); it != r.end(); it++)
	{
		cout << *it << " ";
	}
	current_mse = Compute_MSE();
	
	if ((previous_mse - current_mse) / current_mse <= 0.001) 
	{
		halt_sign = true;
	}
	previous_mse = current_mse;

	cout << " " << endl;
	cout << "Current MSE:  " << current_mse << "    " << "Current PSNR:  " << 10.0 * log10(255.0*255.0 / current_mse) << endl;
	cout << " " << endl;


}

vector<unsigned char> Quantization_1D::Reconstruct_Image(vector<unsigned char> input)
{
	vector<unsigned char> output;
	for (vector<unsigned char>::iterator it = input.begin(); it != input.end(); it++)
	{
		output.push_back(Zero_255_to_U8(Find_New_Value(U8_to_0_255(*it))));
	}
	return output;
}

void Quantization_1D::Computer_PSNR(vector<unsigned char> original, vector<unsigned char> new_file) 
{
	double out = 0;
	for (int i = 0; i < original.size(); i++) 
	{
		out += pow((U8_to_0_255(original[i]) - U8_to_0_255(new_file[i])), 2);
	}
	out = out / original.size();
	out = 10.0 * log10(255.0*255.0 / out);
	cout << "PSNR:  " << out << endl;
}


