#include "fw_blocking_generalized_parallel.h"
#include "constants.h"
#include <algorithm>
#include <iostream>
#include <omp.h>

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

    for (int k = kStart; k < kEnd; ++k) {
        const int kOffset = k * n;

        for (int i = iStart; i < iEnd; ++i) {
            const int iOffset = i * n;
            const WeightType Dik = D[iOffset + k];

            int j = jStart;
            for (; j + 7 < jEnd; j += 8) {
                const WeightType newPath0 = Dik + D[kOffset + j];
                const WeightType newPath1 = Dik + D[kOffset + j + 1];
                const WeightType newPath2 = Dik + D[kOffset + j + 2];
                const WeightType newPath3 = Dik + D[kOffset + j + 3];
                const WeightType newPath4 = Dik + D[kOffset + j + 4];
                const WeightType newPath5 = Dik + D[kOffset + j + 5];
                const WeightType newPath6 = Dik + D[kOffset + j + 6];
                const WeightType newPath7 = Dik + D[kOffset + j + 7];

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

            for (; j < jEnd; ++j) {
                const WeightType newPath = Dik + D[kOffset + j];
                WeightType& cur = D[iOffset + j];
                if (newPath < cur) cur = newPath;
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

void fwBlockingGeneralizedParallel(WeightType* D, int n, int blockSize)
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