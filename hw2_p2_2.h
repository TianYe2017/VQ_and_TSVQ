#pragma once
#ifndef HW2_P2_2_H
#define HW2_P2_2_H
#include "stdafx.h"
#include "io.h"
#include "quantization.h"
#include <vector>
#include <string>
#include <iostream>  
#include <fstream> 

using namespace std;

void HW2_Problem_two_part_two(void);
double Get_Entropy(vector<int> frequency_map, int block_size);


#endif