#pragma once

#include "constants.h"
#include <vector>
#include <string>

std::vector<WeightType> loadGraphBinary(const std::string& filePath, int& n);

bool saveGraphBinary(const std::string& filePath, const std::vector<WeightType>& distMatrix, int n);