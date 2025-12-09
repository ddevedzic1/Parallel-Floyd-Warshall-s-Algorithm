#include "fw_baseline.h"
#include "constants.h"
#include <iostream>

void fwBaseline(WeightType* D, int n, int blockSize)
{
    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Graph size (n) must be between 1 and "
            << MAX_N << ". Algorithm aborted.\n";
        return;
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                const WeightType newPath = D[i * n + k] + D[k * n + j];
                if (newPath < D[i * n + j]) {
                    D[i * n + j] = newPath;
                }
            }
        }
    }
}