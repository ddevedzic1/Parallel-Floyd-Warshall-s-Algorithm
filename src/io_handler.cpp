#include "io_handler.h"
#include <fstream>
#include <iostream>

static_assert(std::is_same<WeightType, int>::value, "WeightType must be 'int' for binary I/O implementation.");

std::vector<WeightType> loadGraphBinary(const std::string& filePath, int& n) {
    n = 0;

    std::ifstream file(filePath, std::ios::binary | std::ios::in);

    if (!file) {
        std::cerr << "ERROR: Cannot open file for reading: " << filePath << "\n";
        return {};
    }

    if (!file.read(reinterpret_cast<char*>(&n), sizeof(n))) {
        std::cerr << "ERROR: Failed to read graph dimension 'n' from file: " << filePath << "\n";
        return {};
    }

    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Invalid graph dimension 'n' read from file (" << n << "). Max allowed: " << MAX_N << "\n";
        n = 0;
        return {};
    }

    size_t dataSize = static_cast<size_t>(n) * n;
    std::vector<WeightType> distMatrix(dataSize);

    if (!file.read(reinterpret_cast<char*>(distMatrix.data()), dataSize * sizeof(WeightType))) {
        std::cerr << "ERROR: Failed to read matrix data from file: " << filePath << "\n";
        n = 0;
        return {};
    }

    return distMatrix;
}

bool saveGraphBinary(const std::string& filePath, const std::vector<WeightType>& distMatrix, int n) {
    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Invalid graph dimension n=" << n << "\n";
        return false;
    }

    size_t expectedSize = static_cast<size_t>(n) * n;
    if (distMatrix.size() != expectedSize) {
        std::cerr << "ERROR: Matrix size mismatch\n";
        return false;
    }

    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "ERROR: Cannot open file: " << filePath << "\n";
        return false;
    }

    if (!file.write(reinterpret_cast<const char*>(&n), sizeof(n))) {
        std::cerr << "ERROR: Failed to write dimension\n";
        return false;
    }

    if (!file.write(reinterpret_cast<const char*>(distMatrix.data()),
        distMatrix.size() * sizeof(WeightType))) {
        std::cerr << "ERROR: Failed to write matrix data\n";
        return false;
    }

    return true;
}