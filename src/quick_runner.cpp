#include "constants.h"
#include <utils.h>
#include "algorithm_runner.h"
#include <iostream>
#include <string>
#include <stdexcept>

int main(int argc, char* argv[])
{
	if (argc < 4 || argc > 6) {
		std::cerr << "Usage: " << argv[0] << " <algorithm_name> <n_size> <density> [seed] [block_size]\n";
		std::cerr << "Example 1 (Baseline): " << argv[0] << " baseline_unroll 1024 0.8\n";
		std::cerr << "Example 2 (Blocking): " << argv[0] << " blocking_parallel_simd 4096 0.9 42 64\n";
		return 1;
	}

	std::string algorithmName = argv[1];
	int n;
	double density;
	unsigned int seed = 0;
	int blockSize = 0;

	try {
		n = std::stoi(argv[2]);
		if (n <= 0 || n > MAX_N) {
			std::cerr << "ERROR: Invalid graph size N (" << n << "). Must be between 1 and " << MAX_N << ".\n";
			return 1;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: Invalid N_SIZE provided (" << argv[1] << "). Must be an integer.\n";
		return 1;
	}

	try {
		density = std::stod(argv[3]);
		if (density < 0.0 || density > 1.0) {
			std::cerr << "ERROR: Invalid density (" << density << "). Must be between 0.0 and 1.0.\n";
			return 1;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: Invalid DENSITY provided (" << argv[2] << "). Must be a floating point number.\n";
		return 1;
	}

	if (argc >= 5) {
		try {
			seed = std::stoul(argv[4]);
		}
		catch (const std::exception& e) {
			std::cerr << "ERROR: Invalid SEED provided (" << argv[4] << "). Must be an unsigned integer. Details: " << e.what() << "\n";
			return 1;
		}
	}

	if (argc == 6) {
		try {
			blockSize = std::stoi(argv[5]);
		}
		catch (const std::exception& e) {
			std::cerr << "ERROR: Invalid BLOCK_SIZE provided (" << argv[5] << "). Must be an integer. Details: " << e.what() << "\n";
			return 1;
		}
	}

	AlgorithmFunc algorithm;
	try {
		algorithm = getAlgorithmFunc(algorithmName);
	}
	catch (const std::out_of_range& e) {
		std::cerr << "ERROR: Invalid algorithm name.\n";
		std::cerr << e.what() << "\n";
		return 1;
	}

	try {
		return AlgorithmRunner::executeQuickMeasurement(algorithm, n, blockSize, density, seed, algorithmName);
	}
	catch (const std::exception& e) {
		std::cerr << "RUNTIME ERROR: An unhandled exception occurred.\n";
		std::cerr << "Details: " << e.what() << "\n";
		return 1;
	}
	catch (...) {
		std::cerr << "RUNTIME ERROR: An unknown error occurred.\n";
		return 1;
	}
}