#include "fw_baseline_simd.h"
#include "constants.h"
#include <iostream>
#include <immintrin.h>
#include <omp.h>

void fwBaselineParallelSimd(WeightType* D, int n, int blockSize)
{
    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Graph size (n) must be between 1 and "
            << MAX_N << ". Algorithm aborted.\n";
        return;
    }

    const int jAlignedEnd = n & ~7;

    for (int k = 0; k < n; ++k) {
        const int kOffset = k * n;

#pragma omp parallel for schedule(guided)
        for (int i = 0; i < n; ++i) {
            const int iOffset = i * n;
            const WeightType Dik = D[iOffset + k];

            if (Dik >= INF) {
                continue;
            }

            const __m256i Dik_vec = _mm256_set1_epi32(Dik);

            int j = 0;
            for (; j < jAlignedEnd; j += 8) {
                __m256i Dkj_vec = _mm256_load_si256((__m256i*) & D[kOffset + j]);
                __m256i Dij_vec = _mm256_load_si256((__m256i*) & D[iOffset + j]);
                __m256i newPath_vec = _mm256_add_epi32(Dik_vec, Dkj_vec);
                __m256i result_vec = _mm256_min_epi32(Dij_vec, newPath_vec);
                _mm256_store_si256((__m256i*) & D[iOffset + j], result_vec);
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