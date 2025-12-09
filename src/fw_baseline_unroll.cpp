#include "fw_baseline_unroll.h"
#include "constants.h"
#include <iostream>

void fwBaselineUnroll(WeightType* D, int n, int blockSize)
{

    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Graph size (n) must be between 1 and "
            << MAX_N << ". Algorithm aborted.\n";
        return;
    }

    for (int k = 0; k < n; ++k) {
        const int kOffset = k * n;

        for (int i = 0; i < n; ++i) {
            const int iOffset = i * n;
            const WeightType Dik = D[iOffset + k];

            if (Dik >= INF) {
                continue;
            }

            int j = 0;
            for (; j + 7 < n; j += 8) {
                const WeightType Dkj0 = D[kOffset + j];
                const WeightType Dkj1 = D[kOffset + j + 1];
                const WeightType Dkj2 = D[kOffset + j + 2];
                const WeightType Dkj3 = D[kOffset + j + 3];
                const WeightType Dkj4 = D[kOffset + j + 4];
                const WeightType Dkj5 = D[kOffset + j + 5];
                const WeightType Dkj6 = D[kOffset + j + 6];
                const WeightType Dkj7 = D[kOffset + j + 7];

                const WeightType newPath0 = Dik + Dkj0;
                const WeightType newPath1 = Dik + Dkj1;
                const WeightType newPath2 = Dik + Dkj2;
                const WeightType newPath3 = Dik + Dkj3;
                const WeightType newPath4 = Dik + Dkj4;
                const WeightType newPath5 = Dik + Dkj5;
                const WeightType newPath6 = Dik + Dkj6;
                const WeightType newPath7 = Dik + Dkj7;

                const int base = iOffset + j;
                if (newPath0 < D[base])     D[base] = newPath0;
                if (newPath1 < D[base + 1]) D[base + 1] = newPath1;
                if (newPath2 < D[base + 2]) D[base + 2] = newPath2;
                if (newPath3 < D[base + 3]) D[base + 3] = newPath3;
                if (newPath4 < D[base + 4]) D[base + 4] = newPath4;
                if (newPath5 < D[base + 5]) D[base + 5] = newPath5;
                if (newPath6 < D[base + 6]) D[base + 6] = newPath6;
                if (newPath7 < D[base + 7]) D[base + 7] = newPath7;
            }

            for (; j < n; ++j) {
                const WeightType newPath = Dik + D[kOffset + j];
                if (newPath < D[iOffset + j]) {
                    D[iOffset + j] = newPath;
                }
            }
        }
    }
}