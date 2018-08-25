#include "stdafx.h"
#include "hw2_p2_2.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream> 


using namespace std;


double Get_Entropy(vector<int> frequency_map, int block_size) 
{   

	vector<double> temp;
	for (int i = 0; i < frequency_map.size(); i++) 
	{ 
		temp.push_back((double)frequency_map[i] / (65536.0 / (block_size * block_size)));
	}
	double entropy = 0.0;
	for (int i = 0; i < frequency_map.size(); i++) 
	{
		if (temp[i] != 0) 
		{
			entropy += -temp[i] * log2(temp[i]);
		}
	}
	return entropy;
}

void HW2_Problem_two_part_two(void)
{

	string path1 = "E:\\MY_LOG\\669\\hw2\\input\\chem.256";
	string path2 = "E:\\MY_LOG\\669\\hw2\\input\\house.256";
	string path3 = "E:\\MY_LOG\\669\\hw2\\input\\moon.256";
	string path4 = "E:\\MY_LOG\\669\\hw2\\input\\couple.256";
	string path5 = "E:\\MY_LOG\\669\\hw2\\input\\f16.256";
	string path6 = "E:\\MY_LOG\\669\\hw2\\input\\elaine.256";

	vector<unsigned char> chem = ReadBinaryFile(path1);
	vector<unsigned char> house = ReadBinaryFile(path2);
	vector<unsigned char> moon = ReadBinaryFile(path3);
	vector<unsigned char> couple = ReadBinaryFile(path4);
	vector<unsigned char> f16 = ReadBinaryFile(path5);
	vector<unsigned char> elaine = ReadBinaryFile(path6);

	Quantization_vector* p = new Quantization_vector(8,64);

	p->Add_New_Image(f16);
	p->Add_New_Image(couple);
	p->Add_New_Image(elaine);
	p->Build_code_book();
	p->Print_code_book();

	p->reset_frequency_map();
	vector<unsigned char> new_chem = p->Reconstruct_Image(chem);
	cout << "Empirical Entropy(chem): " << Get_Entropy(p->Get_Frequence_Map(),8) << endl;

	p->reset_frequency_map();
	vector<unsigned char> new_house = p->Reconstruct_Image(house);
	cout << "Empirical Entropy(house): " << Get_Entropy(p->Get_Frequence_Map(),8) << endl;

	p->reset_frequency_map();
	vector<unsigned char> new_moon = p->Reconstruct_Image(moon);
	cout << "Empirical Entropy(moon): " << Get_Entropy(p->Get_Frequence_Map(),8) << endl;

	p->reset_frequency_map();
	vector<unsigned char> new_couple = p->Reconstruct_Image(couple);
	cout << "Empirical Entropy(couple): " << Get_Entropy(p->Get_Frequence_Map(),8) << endl;

	p->reset_frequency_map();
	vector<unsigned char> new_f16 = p->Reconstruct_Image(f16);
	cout << "Empirical Entropy(f16): " << Get_Entropy(p->Get_Frequence_Map(),8) << endl;

	p->reset_frequency_map();
	vector<unsigned char> new_elaine = p->Reconstruct_Image(elaine); 
	cout << "Empirical Entropy(elaine): " << Get_Entropy(p->Get_Frequence_Map(),8) << endl;

	string P1 = "E:\\MY_LOG\\669\\hw2\\output\\P2_2OUT\\new_chem_stdv.raw";
	string P2 = "E:\\MY_LOG\\669\\hw2\\output\\P2_2OUT\\new_house_stdv.raw";
	string P3 = "E:\\MY_LOG\\669\\hw2\\output\\P2_2OUT\\new_moon_stdv.raw";
	string P4 = "E:\\MY_LOG\\669\\hw2\\output\\P2_2OUT\\new_couple_stdv.raw";
	string P5 = "E:\\MY_LOG\\669\\hw2\\output\\P2_2OUT\\new_f16_stdv.raw";
	string P6 = "E:\\MY_LOG\\669\\hw2\\output\\P2_2OUT\\new_elaine_stdv.raw";

	Write_Image_To_Dat(new_chem, P1);
	p->Computer_PSNR(new_chem, chem);
	Write_Image_To_Dat(new_house, P2);
	p->Computer_PSNR(new_house, house);
	Write_Image_To_Dat(new_moon, P3);
	p->Computer_PSNR(new_moon, moon);
	Write_Image_To_Dat(new_couple, P4);
	p->Computer_PSNR(new_couple, couple);
	Write_Image_To_Dat(new_f16, P5);
	p->Computer_PSNR(new_f16, f16);
	Write_Image_To_Dat(new_elaine, P6);
	p->Computer_PSNR(new_elaine, elaine);





}


