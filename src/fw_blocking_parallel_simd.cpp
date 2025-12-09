#include "fw_blocking_parallel_simd.h"
#include "constants.h"
#include <algorithm>
#include <iostream>
#include <omp.h>
#include <immintrin.h>

static void computeFWBlockSimd(WeightType* D, int n,
    int iStart, int jStart, int kStart,
    int iEnd, int jEnd, int kEnd)
{
    const int jAlignedStart = (jStart + 7) & ~7;
    const int jAlignedEnd = jEnd & ~7;

    for (int k = kStart; k < kEnd; ++k) {
        const int kOffset = k * n;

        for (int i = iStart; i < iEnd; ++i) {
            const int iOffset = i * n;
            const WeightType Dik = D[iOffset + k];
            const __m256i Dik_vec = _mm256_set1_epi32(Dik);

            int j = jStart;
            for (; j < jAlignedStart && j < jEnd; ++j) {
                WeightType newPath = Dik + D[kOffset + j];
                if (newPath < D[iOffset + j]) D[iOffset + j] = newPath;
            }

            if (j < jAlignedEnd) {
                for (; j < jAlignedEnd; j += 8) {
                    __m256i Dkj_vec = _mm256_load_si256((__m256i*) & D[kOffset + j]);
                    __m256i currentPath_vec = _mm256_load_si256((__m256i*) & D[iOffset + j]);

                    __m256i newPath_vec = _mm256_add_epi32(Dik_vec, Dkj_vec);
                    __m256i result_vec = _mm256_min_epi32(currentPath_vec, newPath_vec);

                    _mm256_store_si256((__m256i*) & D[iOffset + j], result_vec);
                }
            }

            for (; j < jEnd; ++j) {
                WeightType newPath = Dik + D[kOffset + j];
                if (newPath < D[iOffset + j]) D[iOffset + j] = newPath;
            }
        }
    }
}

void fwBlockingParallelSimd(WeightType* D, int n, int blockSize)
{
    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Graph size (n) must be between 1 and "
            << MAX_N << ". Algorithm aborted.\n";
        return;
    }

    if (blockSize <= 0) {
        blockSize = 64;
    }

    const int numBlocks = (n + blockSize - 1) / blockSize;

    for (int kBlock = 0; kBlock < numBlocks; ++kBlock) {
        const int kStart = kBlock * blockSize;
        const int kEnd = std::min(kStart + blockSize, n);

        computeFWBlockSimd(D, n, kStart, kStart, kStart, kEnd, kEnd, kEnd);

#pragma omp parallel
        {
#pragma omp for schedule(guided) nowait
            for (int jBlock = 0; jBlock < numBlocks; ++jBlock) {
                if (jBlock == kBlock) continue;
                const int jStart = jBlock * blockSize;
                const int jEnd = std::min(jStart + blockSize, n);
                computeFWBlockSimd(D, n, kStart, jStart, kStart, kEnd, jEnd, kEnd);
            }

#pragma omp for schedule(guided)
            for (int iBlock = 0; iBlock < numBlocks; ++iBlock) {
                if (iBlock == kBlock) continue;
                const int iStart = iBlock * blockSize;
                const int iEnd = std::min(iStart + blockSize, n);
                computeFWBlockSimd(D, n, iStart, kStart, kStart, iEnd, kEnd, kEnd);
            }
        }

        const int blocksBefore = kBlock;
        const int blocksAfter = numBlocks - kBlock - 1;
        const int totalBlocks = blocksBefore + blocksAfter;

#pragma omp parallel for collapse(2) schedule(guided)
        for (int bi = 0; bi < totalBlocks; ++bi) {
            for (int bj = 0; bj < totalBlocks; ++bj) {
                const int iBlock = (bi < blocksBefore) ? bi : (kBlock + 1 + (bi - blocksBefore));
                const int jBlock = (bj < blocksBefore) ? bj : (kBlock + 1 + (bj - blocksBefore));

                const int iStart = iBlock * blockSize;
                const int jStart = jBlock * blockSize;
                const int iEnd = std::min(iStart + blockSize, n);
                const int jEnd = std::min(jStart + blockSize, n);

                computeFWBlockSimd(D, n, iStart, jStart, kStart, iEnd, jEnd, kEnd);
            }
        }
    }
}