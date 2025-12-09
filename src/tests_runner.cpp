#include "tests.h"
#include "utils.h"
#include "fw_baseline.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <algorithm_name> [block_size]\n";
        std::cerr << "Example: " << argv[0] << " blocking_parallel_simd 64\n";
        std::cerr << "\nAvailable algorithms:\n";
        std::cerr << "  baseline, baseline_unroll, baseline_simd, baseline_parallel, baseline_parallel_simd\n";
        std::cerr << "  blocking_baseline, blocking_unroll, blocking_simd, blocking_parallel, blocking_parallel_simd\n";
        std::cerr << "  blocking_generalized_baseline, blocking_generalized_unroll, blocking_generalized_simd,\n";
        std::cerr << "  blocking_generalized_parallel, blocking_generalized_parallel_simd\n";
        return 1;
    }

    std::string algorithmName = argv[1];
    int blockSize = 64;

    if (argc == 3) {
        try {
            blockSize = std::stoi(argv[2]);
            if (blockSize <= 0) {
                std::cerr << "ERROR: block_size must be positive.\n";
                return 1;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid block_size '" << argv[2] << "'.\n";
            return 1;
        }
    }

    AlgorithmFunc algorithm;
    try {
        algorithm = getAlgorithmFunc(algorithmName);
    }
    catch (const std::out_of_range& e) {
        std::cerr << "ERROR: Unknown algorithm '" << algorithmName << "'.\n";
        std::cerr << e.what() << "\n";
        return 1;
    }

    AlgorithmFunc reference = fwBaseline;

    runTests(algorithm, reference, algorithmName, blockSize);

    return 0;
}