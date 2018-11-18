#include "OpenCL.h"
#include <fstream>
#define ARRAY_SIZE 32768
#define LOCAL_SIZE 64
#define PROGRAM_FILE "prime_number_checker.cl"
#define KERNEL_FUNC "test_prime"

#define TEST_DATA_SIZE 2048576

using namespace std;

void main(int argv, string args[])
{
	srand(time(NULL));
	int timesToRun = 1;

	OpenCL openCLTest = OpenCL();
	vector<unsigned int> testData = openCLTest.CreateTestData(TEST_DATA_SIZE);

	cout << "Checking if a random array of " << TEST_DATA_SIZE << " numbers are prime." << endl;


	for (int i = 0; i < timesToRun; ++i)
	{
		clock_t startTime;
		startTime = clock();
		openCLTest.RunSerially(testData);
		clock_t timePassed = clock() - startTime;
		double seconds = timePassed / (double)CLOCKS_PER_SEC;
		cout << "Serial: " << seconds << "s" << endl;
	}

	for (int i = 0; i < timesToRun; ++i)
	{
		clock_t startTime;
		startTime = clock();
		openCLTest.RunOpenCL(testData, 1);
		clock_t timePassed = clock() - startTime;
		double seconds = timePassed / (double)CLOCKS_PER_SEC;
		cout << "OpenCL CPU: " << seconds << "s" << endl;
	}

	for (int i = 0; i < timesToRun; ++i)
	{
		clock_t startTime;
		startTime = clock();
		openCLTest.RunOpenCL(testData, 0);
		clock_t timePassed = clock() - startTime;
		double seconds = timePassed / (double)CLOCKS_PER_SEC;
		cout << "OpenCL GPU: " << seconds << "s" << endl;
	}

	for (int i = 0; i < timesToRun; ++i)
	{
		clock_t startTime;
		startTime = clock();
		openCLTest.RunOpenCL(testData, 2);
		clock_t timePassed = clock() - startTime;
		double seconds = timePassed / (double)CLOCKS_PER_SEC;
		cout << "OpenCL CPU & GPU: " << seconds << "s" << endl;
	}

	getchar();
}

OpenCL::OpenCL()
{
}

OpenCL::~OpenCL()
{
}

vector<unsigned int> OpenCL::CreateTestData(int sizeOfArrays)
{
	vector<unsigned int> testData = vector<unsigned int>();
	for (int j = 0; j < sizeOfArrays; ++j)
	{
		//int uint_max = numeric_limits<int>::max();
		int randNumber = rand(); // 0 - 32767

		testData.push_back(randNumber);
	}

	return testData;
}

void OpenCL::RunSerially(vector<unsigned int> data)
{
	vector<unsigned int> numOfPrimeList = GetAmountOfPrimes(data);

	int numberOfPrimes = 0;
	vector<unsigned int>::iterator iter = numOfPrimeList.begin();
	while (iter != numOfPrimeList.end())
	{
		numberOfPrimes += *iter;

		++iter;
	}
	cout << "Detected " << numberOfPrimes << " prime numbers." << endl;
}

vector<unsigned int> OpenCL::GetAmountOfPrimes(vector<unsigned int> numberList)
{
	vector<unsigned int> numOfPrimesList = vector<unsigned int>();
	for (int i = 0; i < numberList.size(); ++i)
	{
		int numOfPrimes = 0;
		if (CheckIfPrime(numberList[i]))
			++numOfPrimes;
		numOfPrimesList.push_back(numOfPrimes);
	}

	return numOfPrimesList;
}

bool OpenCL::CheckIfPrime(unsigned int possiblePrime)
{
	if (possiblePrime <= 1)
		return false;
	else if (possiblePrime <= 3)
		return true;
	else if (possiblePrime % 2 == 0 || possiblePrime % 3 == 0)
		return false;

	for (unsigned int i = ceil(possiblePrime / 2); i > 1; --i)
	{
		if (possiblePrime % i == 0)
		{
			return false;
		}
	}
	return true;
}

// OpenCL Logic
void OpenCL::RunOpenCL(vector<unsigned int> data, int deviceID)
{
	//Create device and context
	// Platforms are like Intel, AMD, NVIDIA
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();

	// Device is GPU/CPU
	vector<cl::Device> devices;
	if (deviceID == 0)
	{
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	}
	else if (deviceID == 1)
	{
		platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
	}
	else
	{
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	}

	auto device = devices.front();

	// Sources are just a pair of strings/size of that string that represent the file name of
	// the kernels (you can have more than one kernel)
	ifstream testFile("prime_number_checker.cl");
	string src(istreambuf_iterator<char>(testFile), (istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, make_pair(src.c_str(), src.length() + 1));

	// Context is used to change devices(?)
	cl::Context context(device);

	cl::Program program(context, sources);
	auto err = program.build("-cl-std=CL1.2");

	vector<unsigned int> output = vector<unsigned int>(data.size());

	cl::Buffer inBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * data.size(), data.data());
	cl::Buffer outBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(unsigned int) * output.size());
	cl::Kernel kernel(program, KERNEL_FUNC, &err);
	kernel.setArg(0, inBuffer);
	kernel.setArg(1, outBuffer);

	cl::CommandQueue queue(context, device);

	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(output.size()));
	queue.enqueueReadBuffer(outBuffer, CL_FALSE, 0, sizeof(unsigned int) * output.size(), output.data());

	queue.finish();
}