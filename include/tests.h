#pragma once

#include "utils.h"
#include <string>

void runTests(AlgorithmFunc algorithm, AlgorithmFunc reference,
    const std::string& name, int blockSize);