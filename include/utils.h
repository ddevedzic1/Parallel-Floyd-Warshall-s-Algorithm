#pragma once

#include "constants.h"
#include "algorithm_runner.h"
#include <string>
#include <map>

bool hasNegativeCycle(const WeightType* distMatrix, int n);

extern const std::map<std::string, AlgorithmFunc> ALGORITHM_MAP;

AlgorithmFunc getAlgorithmFunc(const std::string& algorithmName);