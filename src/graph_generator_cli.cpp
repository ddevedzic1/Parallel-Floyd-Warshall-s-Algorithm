#include "constants.h"
#include "io_handler.h"
#include "graph_generator.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <filesystem>
#include <random>
#include <iomanip>

int main(int argc, char* argv[]) {
	if (argc < 3 || argc > 4) {
		std::cerr << "Usage: " << argv[0] << " <n_size> <density> [seed]\n";
		std::cerr << "Note: Density will be rounded to the nearest integer percent (e.g., 0.864 -> 86%) for the filename.\n";
		std::cerr << "Example 1 (Non-reproducible):" << argv[0] << " 1024 0.8 42\n";
		std::cerr << "Example 2 (Reproducible): " << argv[0] << " 1024 0.8\n";
		return 1;
	}

	int n;
	double density;
	unsigned int seed = 0;

	try {
		n = std::stoi(argv[1]);
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
		density = std::stod(argv[2]);
		if (density < 0.0 || density > 1.0) {
			std::cerr << "ERROR: Invalid density (" << density << "). Must be between 0.0 and 1.0.\n";
			return 1;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: Invalid DENSITY provided (" << argv[2] << "). Must be a floating point number.\n";
		return 1;
	}

	if (argc == 4) {
		try {
			seed = std::stoul(argv[3]);
		}
		catch (const std::exception& e) {
			std::cerr << "ERROR: Invalid SEED provided (" << argv[3] << "). Must be an unsigned integer. Details: " << e.what() << "\n";
			return 1;
		}
	}

	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	if (seed == 0) {
		seed = 1;
	}

	if (!std::filesystem::exists("data")) {
		if (!std::filesystem::create_directories("data")) {
			std::cerr << "ERROR: Failed to create data directory.\n";
			return 1;
		}
	}

	std::cout << "Generating graph...\n";
	std::vector<WeightType> distMatrix = generateGraph1D(n, density, seed);

	if (distMatrix.empty()) {
		std::cerr << "ERROR: Graph generation failed.\n";
		return 1;
	}

	std::stringstream ss;

	int densityPercent = static_cast<int>(std::round(density * 100.0));

	ss << "data/graph_n" << n << "_d" << densityPercent << "_s" << seed << ".bin";
	std::string filePath = ss.str();

	std::cout << "Saving graph to binary file: " << filePath << "...\n";

	if (saveGraphBinary(filePath, distMatrix, n)) {
		std::cout << "SUCCESS: Graph " << n << "x" << n
			<< " (Density: " << std::fixed << std::setprecision(2) << (density * 100.0)
			<< "%, Seed: " << seed << ") saved successfully.\n";
		return 0;
	}

	std::cerr << "FAILURE: Saving graph to file failed.\n";
	return 1;
}