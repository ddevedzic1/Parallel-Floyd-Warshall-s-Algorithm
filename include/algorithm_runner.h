#pragma once

#include <string>
#include "constants.h"

using AlgorithmFunc = void(*)(WeightType* D, int n, int blockSize);

namespace AlgorithmRunner {
	int executeQuickMeasurement(AlgorithmFunc algorithm, int n, int blockSize, double density, unsigned int seed, const std::string& algorithm_name);
	int runAlgorithmOnLoadedGraph(AlgorithmFunc algorithm, const std::string& graphFilePath, int blockSize);
}