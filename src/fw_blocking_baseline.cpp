#include "fw_blocking_baseline.h"
#include "constants.h"
#include <algorithm>
#include <iostream>

static void computeFWBlock(
    WeightType* D, int n,
    int iStart, int jStart, int kStart,
    int iEnd, int jEnd, int kEnd)
{
    for (int k = kStart; k < kEnd; ++k) {
        const int kOffset = k * n;

        for (int i = iStart; i < iEnd; ++i) {
            const int iOffset = i * n;
            const WeightType Dik = D[iOffset + k];

            for (int j = jStart; j < jEnd; ++j) {
                const WeightType newPath = Dik + D[kOffset + j];
                if (newPath < D[iOffset + j]) {
                    D[iOffset + j] = newPath;
                }
            }
        }
    }
}

void fwBlockingBaseline(WeightType* D, int n, int blockSize)
{
    if (n <= 0 || n > MAX_N) {
        std::cerr << "ERROR: Graph size (n) must be between 1 and "
            << MAX_N << ". Algorithm aborted.\n";
        return;
    }

    if (blockSize <= 0) {
        if (n <= 2046) {
            blockSize = n / 2;
            if (blockSize < 1) blockSize = 1;
        }
        else {
            blockSize = 1024;
        }
    }

    const int numBlocks = (n + blockSize - 1) / blockSize;

    for (int kBlock = 0; kBlock < numBlocks; ++kBlock) {
        const int kStart = kBlock * blockSize;
        const int kEnd = std::min(kStart + blockSize, n);

        computeFWBlock(D, n, kStart, kStart, kStart, kEnd, kEnd, kEnd);

        for (int jBlock = 0; jBlock < numBlocks; ++jBlock) {
            if (jBlock == kBlock) continue;
            const int jStart = jBlock * blockSize;
            const int jEnd = std::min(jStart + blockSize, n);
            computeFWBlock(D, n, kStart, jStart, kStart, kEnd, jEnd, kEnd);
        }

        for (int iBlock = 0; iBlock < numBlocks; ++iBlock) {
            if (iBlock == kBlock) continue;
            const int iStart = iBlock * blockSize;
            const int iEnd = std::min(iStart + blockSize, n);
            computeFWBlock(D, n, iStart, kStart, kStart, iEnd, kEnd, kEnd);
        }

        for (int iBlock = 0; iBlock < numBlocks; ++iBlock) {
            if (iBlock == kBlock) continue;
            const int iStart = iBlock * blockSize;
            const int iEnd = std::min(iStart + blockSize, n);

            for (int jBlock = 0; jBlock < numBlocks; ++jBlock) {
                if (jBlock == kBlock) continue;
                const int jStart = jBlock * blockSize;
                const int jEnd = std::min(jStart + blockSize, n);
                computeFWBlock(D, n, iStart, jStart, kStart, iEnd, jEnd, kEnd);
            }
        }
    }
}