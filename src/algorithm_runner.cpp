#include "algorithm_runner.h"
#include "constants.h"
#include "utils.h"
#include "io_handler.h"
#include "fw_baseline.h"
#include "graph_generator.h"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace AlgorithmRunner {
	int executeQuickMeasurement(AlgorithmFunc algorithm, int n, int blockSize, double density, unsigned int seed, const std::string& algorithm_name)
	{
		if (n <= 0 || n > MAX_N) {
			std::cerr << "ERROR: Graph size (n) must be between 1 and " << MAX_N << " Aborting test run.\n";
			return 1;
		}

		size_t size_n = static_cast<size_t>(n);

		std::cout << "\n====================================\n";
		std::cout << "QUICK MEASUREMENT: " << algorithm_name << "\n";
		std::cout << "====================================\n";
		std::cout << "Graph size: " << n << " vertices\n";
		std::cout << "Edge density: " << (density * 100.0) << "%\n";
		if (seed == 0) {
			std::cout << "Seed: Random (Non-reproducible)\n\n";
		}
		else {
			std::cout << "Seed: " << seed << " (Reproducible)\n\n";
		}

		std::cout << "Generating graph (1D array) on the fly...\n";

		/*std::vector<WeightType> D_data = generateGraph1D(n, density, seed);

		if (D_data.size() != size_n * size_n) {
			std::cerr << "ERROR: Graph data integrity failed. Expected size: "
				<< (size_n * size_n) << ", Actual size: " << D_data.size()
				<< ". Aborting test run.\n";
			return 1;
		}

		WeightType* D = D_data.data();*/

		WeightType* D = generateAlignedGraph1D(n, density, seed);

		std::cout << "Running " << algorithm_name << "...\n";

		auto start = std::chrono::steady_clock::now();
		algorithm(D, n, blockSize);
		auto end = std::chrono::steady_clock::now();

		std::chrono::duration<double> elapsed = end - start;

		std::cout << "\nResults:\n";
		std::cout << "--------\n";
		std::cout << "Execution time: " << std::fixed << std::setprecision(6)
			<< elapsed.count() << " seconds\n";

		bool negativeCycleDetected = hasNegativeCycle(D, n);

		std::cout << "Validation check: ";
		if (negativeCycleDetected) {
			std::cout << "FAILED (Graph contains a negative cycle).\n";
		}
		else {
			std::cout << "PASSED (No negative cycles detected).\n";
		}

		_aligned_free(D);

		return 0;
	}

	int runAlgorithmOnLoadedGraph(AlgorithmFunc algorithm, const std::string& graphFilePath, int blockSize) {
		int n = 0;

		std::vector<WeightType> distMatrix = loadGraphBinary(graphFilePath, n);

		if (n <= 0) {
			return 1;
		}

		WeightType* D = distMatrix.data();

		algorithm(D, n, blockSize);

		return 0;
	}
}

