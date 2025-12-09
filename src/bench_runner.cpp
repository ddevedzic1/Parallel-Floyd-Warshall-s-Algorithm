#include "utils.h"
#include "algorithm_runner.h"
#include <iostream>
#include <string>
#include <stdexcept>

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <algorithm_name> <graph_file_path> [block_size]\n";
        std::cerr << "Example 1 (Baseline): " << argv[0] << " baseline_unroll data/graph_n2048_d90_s42.bin\n";
        std::cerr << "Example 2 (Blocking): " << argv[0] << " blocking_simd data/graph_n4096_d80_s333.bin 64\n";
        return 1;
    }

    std::string algorithmName = argv[1];
    const std::string graphFilePath = argv[2];
    int blockSize = 0;

    if (argc == 4) {
        try {
            blockSize = std::stoi(argv[3]);
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid BLOCK_SIZE provided (" << argv[3] << "). Must be an integer. Details: " << e.what() << "\n";
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
        return AlgorithmRunner::runAlgorithmOnLoadedGraph(algorithm, graphFilePath, blockSize);
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