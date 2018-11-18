#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <math.h>
#include <limits>
#include <iostream>
#include <vector>
#include <time.h> 
#include <CL/cl.hpp>

using namespace std;

class OpenCL
{
public:
	OpenCL();
	~OpenCL();
	bool CheckIfPrime(unsigned int potentialPrime);
	void RunOpenCL(vector<unsigned int> data, int device);
	void RunSerially(vector<unsigned int> data);
	vector<unsigned int> CreateTestData(int sizeOfArrays);
private:
	vector<unsigned int> GetAmountOfPrimes(vector<unsigned int> numberList);
};
