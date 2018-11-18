__kernel void test_prime(__global uint* inData, __global uint* outData) {
	uint possiblePrime = inData[get_global_id(0)];
	uint isPrime = 0;
	bool breakPoint = false;

	if (possiblePrime <= 1)
	{
		breakPoint = true;
	}
	else if (possiblePrime <= 3)
	{
		isPrime = 1;
		breakPoint = true;
	}
	else if (possiblePrime % 2 == 0 || possiblePrime % 3 == 0)
	{
		breakPoint = true;
	}

	uint result = 0;
	if (possiblePrime % 2 == 0)
	{
		result = possiblePrime / 2;
	}
	else
	{
		result = possiblePrime / 2 + 1;
	}

	for(uint i = result; i > 1 && breakPoint == false; --i)
	{
		if (possiblePrime % i == 0)
		{
			isPrime = 0;
			breakPoint = true;
		}
	}
	if (breakPoint == false)
	{
		isPrime = 1;
	}


	outData[get_global_id(0)] = isPrime;
}