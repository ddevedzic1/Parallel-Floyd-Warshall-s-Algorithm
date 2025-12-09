#pragma once

#include <vector>
#include "constants.h"

WeightType* generateAlignedGraph1D(int n, double density, unsigned int seed = 0);

std::vector<WeightType> generateGraph1D(int n, double density, unsigned int seed = 0);