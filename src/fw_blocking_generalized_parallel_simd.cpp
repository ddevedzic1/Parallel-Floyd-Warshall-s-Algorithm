#include "fw_blocking_generalized_parallel_simd.h"
#include "constants.h"
#include <algorithm>
#include <iostream>
#include <omp.h>
#include <immintrin.h>

static void computeTile(
    WeightType* D, int n,
    int kBlock, int iBlock, int jBlock,
    int blockSize)
{
    const int kStart = kBlock * blockSize;
    const int iStart = iBlock * blockSize;
    const int jStart = jBlock * blockSize;
    const int kEnd = std::min(kStart + blockSize, n);
    const int iEnd = std::min(iStart + blockSize, n);
    const int jEnd = std::min(jStart + blockSize, n);

    const int jAlignedStart = (jStart + 7) & ~7;
    const int jAlignedEnd = jEnd & ~7;

    for (int k = kStart; k < kEnd; ++k) {
        const int kOffset = k * n;

        for (int i = iStart; i < iEnd; ++i) {
            const int iOffset = i * n;
            const WeightType Dik = D[iOffset + k];

            if (Dik >= INF) {
                continue;
            }

            int j = jStart;

            for (; j < jAlignedStart && j < jEnd; ++j) {
                const WeightType newPath = Dik + D[kOffset + j];
                if (newPath < D[iOffset + j]) {
                    D[iOffset + j] = newPath;
                }
            }

            if (j < jAlignedEnd) {
                const __m256i Dik_vec = _mm256_set1_epi32(Dik);

                for (; j < jAlignedEnd; j += 8) {
                    __m256i Dkj_vec = _mm256_load_si256((__m256i*) & D[kOffset + j]);
                    __m256i currentPath_vec = _mm256_load_si256((__m256i*) & D[iOffset + j]);
                    __m256i newPath_vec = _mm256_add_epi32(Dik_vec, Dkj_vec);
                    __m256i result_vec = _mm256_min_epi32(currentPath_vec, newPath_vec);
                    _mm256_store_si256((__m256i*) & D[iOffset + j], result_vec);
                }
            }

            for (; j < jEnd; ++j) {
                const WeightType newPath = Dik + D[kOffset + j];
                if (newPath < D[iOffset + j]) {
                    D[iOffset + j] = newPath;
                }
            }
        }
    }
}

static void calcLeadBlock(
    WeightType* D, int n,
    int kBlockBase, int l,
    int blockSize)
{
    const int currentK = kBlockBase + l;

    if (l == 0) {
        computeTile(D, n, currentK, currentK, currentK, blockSize);
    }
    else {
        for (int m = 0; m < l; ++m) {
            computeTile(D, n, kBlockBase + m, currentK, currentK, blockSize);
        }

        computeTile(D, n, currentK, currentK, currentK, blockSize);
    }
}

static void calcLeadRowAndColumn(
    WeightType* D, int n,
    int kBlockBase, int l,
    int blockSize, int numBlocks)
{
    const int currentK = kBlockBase + l;


#pragma omp parallel
    {
#pragma omp for schedule(guided) nowait
        for (int jBlock = 0; jBlock < numBlocks; ++jBlock) {
            if (jBlock == currentK) {
                continue;
            }

            if (l == 0) {
                computeTile(D, n, currentK, currentK, jBlock, blockSize);
            }
            else {
                int mStart;
                if (jBlock >= kBlockBase && jBlock < currentK) {
                    mStart = jBlock - kBlockBase + 1;
                }
                else {
                    mStart = 0;
                }

                for (int m = mStart; m < l; ++m) {
                    computeTile(D, n, kBlockBase + m, currentK, jBlock, blockSize);
                }

                computeTile(D, n, currentK, currentK, jBlock, blockSize);
            }
        }

#pragma omp for schedule(guided)
        for (int iBlock = 0; iBlock < numBlocks; ++iBlock) {
            if (iBlock == currentK) {
                continue;
            }

            if (l == 0) {
                computeTile(D, n, currentK, iBlock, currentK, blockSize);
            }
            else {
                int mStart;
                if (iBlock >= kBlockBase && iBlock < currentK) {
                    mStart = iBlock - kBlockBase + 1;
                }
                else {
                    mStart = 0;
                }

                for (int m = mStart; m < l; ++m) {
                    computeTile(D, n, kBlockBase + m, iBlock, currentK, blockSize);
                }

                computeTile(D, n, currentK, iBlock, currentK, blockSize);
            }
        }
    }
}

static void calcLeadRowAndColumnReverse(
    WeightType* D, int n,
    int kBlockBase, int kappa, int l,
    int blockSize, int numBlocks)
{
    const int currentK = kBlockBase + l;

    if (l + 1 >= kappa) {
        return;
    }

#pragma omp parallel
    {

#pragma omp for schedule(guided) nowait
        for (int jBlock = 0; jBlock <= currentK; ++jBlock) {
            for (int m = l + 1; m < kappa; ++m) {
                computeTile(D, n, kBlockBase + m, currentK, jBlock, blockSize);
            }
        }

#pragma omp for schedule(guided) nowait
        for (int jBlock = kBlockBase + kappa; jBlock < numBlocks; ++jBlock) {
            for (int m = l + 1; m < kappa; ++m) {
                computeTile(D, n, kBlockBase + m, currentK, jBlock, blockSize);
            }
        }

#pragma omp for schedule(guided) nowait
        for (int iBlock = 0; iBlock < currentK; ++iBlock) {
            for (int m = l + 1; m < kappa; ++m) {
                computeTile(D, n, kBlockBase + m, iBlock, currentK, blockSize);
            }
        }

#pragma omp for schedule(guided)
        for (int iBlock = kBlockBase + kappa; iBlock < numBlocks; ++iBlock) {
            for (int m = l + 1; m < kappa; ++m) {
                computeTile(D, n, kBlockBase + m, iBlock, currentK, blockSize);
            }
        }
    }
}

static void calcRestBlocks(
    WeightType* D, int n,
    int kBlockBase, int kappa,
    int blockSize, int numBlocks)
{
    const int kappaEnd = kBlockBase + kappa;

    const int blocksBefore = kBlockBase;
    const int blocksAfter = numBlocks - kappaEnd;
    const int totalBlocks = blocksBefore + blocksAfter;

#pragma omp parallel for collapse(2) schedule(guided)
    for (int bi = 0; bi < totalBlocks; ++bi) {
        for (int bj = 0; bj < totalBlocks; ++bj) {
            const int iBlock = (bi < blocksBefore) ? bi : (kappaEnd + (bi - blocksBefore));
            const int jBlock = (bj < blocksBefore) ? bj : (kappaEnd + (bj - blocksBefore));

            for (int m = 0; m < kappa; ++m) {
                computeTile(D, n, kBlockBase + m, iBlock, jBlock, blockSize);
            }
        }
    }
}

void fwBlockingGeneralizedParallelSimd(WeightType* D, int n, int blockSize)
{
    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Graph size (n) must be between 1 and "
            << MAX_N << ". Algorithm aborted.\n";
        return;
    }

    if (blockSize <= 0) {
        blockSize = 88;
    }

    const int kappa = 2;
    const int numBlocks = (n + blockSize - 1) / blockSize;

    for (int kBlockBase = 0; kBlockBase < numBlocks; kBlockBase += kappa) {
        const int blocksInStage = std::min(kappa, numBlocks - kBlockBase);

        for (int l = 0; l < blocksInStage; ++l) {
            calcLeadBlock(D, n, kBlockBase, l, blockSize);
            calcLeadRowAndColumn(D, n, kBlockBase, l, blockSize, numBlocks);
        }

        for (int l = blocksInStage - 2; l >= 0; --l) {
            calcLeadRowAndColumnReverse(D, n, kBlockBase, blocksInStage, l,
                blockSize, numBlocks);
        }

        if (blocksInStage < numBlocks) {
            calcRestBlocks(D, n, kBlockBase, blocksInStage, blockSize, numBlocks);
        }
    }
}