#include "fw_blocking_unroll.h"
#include "constants.h"
#include <algorithm>
#include <iostream>

static void computeFWBlockUnroll(
        WeightType*  D, int n,
    int iStart, int jStart, int kStart,
    int iEnd, int jEnd, int kEnd)
{
    for (int k = kStart; k < kEnd; ++k) {
        const int kOffset = k * n;
        int i = iStart;

        for (; i + 7 < iEnd; i += 8) {
            const int iOffset0 = i * n;
            const int iOffset1 = (i + 1) * n;
            const int iOffset2 = (i + 2) * n;
            const int iOffset3 = (i + 3) * n;
            const int iOffset4 = (i + 4) * n;
            const int iOffset5 = (i + 5) * n;
            const int iOffset6 = (i + 6) * n;
            const int iOffset7 = (i + 7) * n;

            const WeightType Dik0 = D[iOffset0 + k];
            const WeightType Dik1 = D[iOffset1 + k];
            const WeightType Dik2 = D[iOffset2 + k];
            const WeightType Dik3 = D[iOffset3 + k];
            const WeightType Dik4 = D[iOffset4 + k];
            const WeightType Dik5 = D[iOffset5 + k];
            const WeightType Dik6 = D[iOffset6 + k];
            const WeightType Dik7 = D[iOffset7 + k];


            int j = jStart;
            for (; j + 7 < jEnd; j += 8) {
                const WeightType Dkj0 = D[kOffset + j];
                const WeightType Dkj1 = D[kOffset + j + 1];
                const WeightType Dkj2 = D[kOffset + j + 2];
                const WeightType Dkj3 = D[kOffset + j + 3];
                const WeightType Dkj4 = D[kOffset + j + 4];
                const WeightType Dkj5 = D[kOffset + j + 5];
                const WeightType Dkj6 = D[kOffset + j + 6];
                const WeightType Dkj7 = D[kOffset + j + 7];

                const int base = iOffset0 + j;

                if (Dik0 < INF) {
                    const WeightType newPath0 = Dik0 + Dkj0;
                    const WeightType newPath1 = Dik0 + Dkj1;
                    const WeightType newPath2 = Dik0 + Dkj2;
                    const WeightType newPath3 = Dik0 + Dkj3;
                    const WeightType newPath4 = Dik0 + Dkj4;
                    const WeightType newPath5 = Dik0 + Dkj5;
                    const WeightType newPath6 = Dik0 + Dkj6;
                    const WeightType newPath7 = Dik0 + Dkj7;

                    if (newPath0 < D[base]) D[base] = newPath0;
                    if (newPath1 < D[base + 1]) D[base + 1] = newPath1;
                    if (newPath2 < D[base + 2]) D[base + 2] = newPath2;
                    if (newPath3 < D[base + 3]) D[base + 3] = newPath3;
                    if (newPath4 < D[base + 4]) D[base + 4] = newPath4;
                    if (newPath5 < D[base + 5]) D[base + 5] = newPath5;
                    if (newPath6 < D[base + 6]) D[base + 6] = newPath6;
                    if (newPath7 < D[base + 7]) D[base + 7] = newPath7;
                }

                const int base1 = iOffset1 + j;
                if (Dik1 < INF) {
                    const WeightType newPath0 = Dik1 + Dkj0;
                    const WeightType newPath1 = Dik1 + Dkj1;
                    const WeightType newPath2 = Dik1 + Dkj2;
                    const WeightType newPath3 = Dik1 + Dkj3;
                    const WeightType newPath4 = Dik1 + Dkj4;
                    const WeightType newPath5 = Dik1 + Dkj5;
                    const WeightType newPath6 = Dik1 + Dkj6;
                    const WeightType newPath7 = Dik1 + Dkj7;

                    if (newPath0 < D[base1]) D[base1] = newPath0;
                    if (newPath1 < D[base1 + 1]) D[base1 + 1] = newPath1;
                    if (newPath2 < D[base1 + 2]) D[base1 + 2] = newPath2;
                    if (newPath3 < D[base1 + 3]) D[base1 + 3] = newPath3;
                    if (newPath4 < D[base1 + 4]) D[base1 + 4] = newPath4;
                    if (newPath5 < D[base1 + 5]) D[base1 + 5] = newPath5;
                    if (newPath6 < D[base1 + 6]) D[base1 + 6] = newPath6;
                    if (newPath7 < D[base1 + 7]) D[base1 + 7] = newPath7;
                }

                const int base2 = iOffset2 + j;
                if (Dik2 < INF) {
                    const WeightType newPath0 = Dik2 + Dkj0;
                    const WeightType newPath1 = Dik2 + Dkj1;
                    const WeightType newPath2 = Dik2 + Dkj2;
                    const WeightType newPath3 = Dik2 + Dkj3;
                    const WeightType newPath4 = Dik2 + Dkj4;
                    const WeightType newPath5 = Dik2 + Dkj5;
                    const WeightType newPath6 = Dik2 + Dkj6;
                    const WeightType newPath7 = Dik2 + Dkj7;

                    if (newPath0 < D[base2]) D[base2] = newPath0;
                    if (newPath1 < D[base2 + 1]) D[base2 + 1] = newPath1;
                    if (newPath2 < D[base2 + 2]) D[base2 + 2] = newPath2;
                    if (newPath3 < D[base2 + 3]) D[base2 + 3] = newPath3;
                    if (newPath4 < D[base2 + 4]) D[base2 + 4] = newPath4;
                    if (newPath5 < D[base2 + 5]) D[base2 + 5] = newPath5;
                    if (newPath6 < D[base2 + 6]) D[base2 + 6] = newPath6;
                    if (newPath7 < D[base2 + 7]) D[base2 + 7] = newPath7;
                }

                const int base3 = iOffset3 + j;
                if (Dik3 < INF) {
                    const WeightType newPath0 = Dik3 + Dkj0;
                    const WeightType newPath1 = Dik3 + Dkj1;
                    const WeightType newPath2 = Dik3 + Dkj2;
                    const WeightType newPath3 = Dik3 + Dkj3;
                    const WeightType newPath4 = Dik3 + Dkj4;
                    const WeightType newPath5 = Dik3 + Dkj5;
                    const WeightType newPath6 = Dik3 + Dkj6;
                    const WeightType newPath7 = Dik3 + Dkj7;

                    if (newPath0 < D[base3]) D[base3] = newPath0;
                    if (newPath1 < D[base3 + 1]) D[base3 + 1] = newPath1;
                    if (newPath2 < D[base3 + 2]) D[base3 + 2] = newPath2;
                    if (newPath3 < D[base3 + 3]) D[base3 + 3] = newPath3;
                    if (newPath4 < D[base3 + 4]) D[base3 + 4] = newPath4;
                    if (newPath5 < D[base3 + 5]) D[base3 + 5] = newPath5;
                    if (newPath6 < D[base3 + 6]) D[base3 + 6] = newPath6;
                    if (newPath7 < D[base3 + 7]) D[base3 + 7] = newPath7;
                }

                const int base4 = iOffset4 + j;
                if (Dik4 < INF) {
                    const WeightType newPath0 = Dik4 + Dkj0;
                    const WeightType newPath1 = Dik4 + Dkj1;
                    const WeightType newPath2 = Dik4 + Dkj2;
                    const WeightType newPath3 = Dik4 + Dkj3;
                    const WeightType newPath4 = Dik4 + Dkj4;
                    const WeightType newPath5 = Dik4 + Dkj5;
                    const WeightType newPath6 = Dik4 + Dkj6;
                    const WeightType newPath7 = Dik4 + Dkj7;

                    if (newPath0 < D[base4]) D[base4] = newPath0;
                    if (newPath1 < D[base4 + 1]) D[base4 + 1] = newPath1;
                    if (newPath2 < D[base4 + 2]) D[base4 + 2] = newPath2;
                    if (newPath3 < D[base4 + 3]) D[base4 + 3] = newPath3;
                    if (newPath4 < D[base4 + 4]) D[base4 + 4] = newPath4;
                    if (newPath5 < D[base4 + 5]) D[base4 + 5] = newPath5;
                    if (newPath6 < D[base4 + 6]) D[base4 + 6] = newPath6;
                    if (newPath7 < D[base4 + 7]) D[base4 + 7] = newPath7;
                }

                const int base5 = iOffset5 + j;
                if (Dik5 < INF) {
                    const WeightType newPath0 = Dik5 + Dkj0;
                    const WeightType newPath1 = Dik5 + Dkj1;
                    const WeightType newPath2 = Dik5 + Dkj2;
                    const WeightType newPath3 = Dik5 + Dkj3;
                    const WeightType newPath4 = Dik5 + Dkj4;
                    const WeightType newPath5 = Dik5 + Dkj5;
                    const WeightType newPath6 = Dik5 + Dkj6;
                    const WeightType newPath7 = Dik5 + Dkj7;

                    if (newPath0 < D[base5]) D[base5] = newPath0;
                    if (newPath1 < D[base5 + 1]) D[base5 + 1] = newPath1;
                    if (newPath2 < D[base5 + 2]) D[base5 + 2] = newPath2;
                    if (newPath3 < D[base5 + 3]) D[base5 + 3] = newPath3;
                    if (newPath4 < D[base5 + 4]) D[base5 + 4] = newPath4;
                    if (newPath5 < D[base5 + 5]) D[base5 + 5] = newPath5;
                    if (newPath6 < D[base5 + 6]) D[base5 + 6] = newPath6;
                    if (newPath7 < D[base5 + 7]) D[base5 + 7] = newPath7;
                }

                const int base6 = iOffset6 + j;
                if (Dik6 < INF) {
                    const WeightType newPath0 = Dik6 + Dkj0;
                    const WeightType newPath1 = Dik6 + Dkj1;
                    const WeightType newPath2 = Dik6 + Dkj2;
                    const WeightType newPath3 = Dik6 + Dkj3;
                    const WeightType newPath4 = Dik6 + Dkj4;
                    const WeightType newPath5 = Dik6 + Dkj5;
                    const WeightType newPath6 = Dik6 + Dkj6;
                    const WeightType newPath7 = Dik6 + Dkj7;

                    if (newPath0 < D[base6]) D[base6] = newPath0;
                    if (newPath1 < D[base6 + 1]) D[base6 + 1] = newPath1;
                    if (newPath2 < D[base6 + 2]) D[base6 + 2] = newPath2;
                    if (newPath3 < D[base6 + 3]) D[base6 + 3] = newPath3;
                    if (newPath4 < D[base6 + 4]) D[base6 + 4] = newPath4;
                    if (newPath5 < D[base6 + 5]) D[base6 + 5] = newPath5;
                    if (newPath6 < D[base6 + 6]) D[base6 + 6] = newPath6;
                    if (newPath7 < D[base6 + 7]) D[base6 + 7] = newPath7;
                }

                const int base7 = iOffset7 + j;
                if (Dik7 < INF) {
                    const WeightType newPath0 = Dik7 + Dkj0;
                    const WeightType newPath1 = Dik7 + Dkj1;
                    const WeightType newPath2 = Dik7 + Dkj2;
                    const WeightType newPath3 = Dik7 + Dkj3;
                    const WeightType newPath4 = Dik7 + Dkj4;
                    const WeightType newPath5 = Dik7 + Dkj5;
                    const WeightType newPath6 = Dik7 + Dkj6;
                    const WeightType newPath7 = Dik7 + Dkj7;

                    if (newPath0 < D[base7]) D[base7] = newPath0;
                    if (newPath1 < D[base7 + 1]) D[base7 + 1] = newPath1;
                    if (newPath2 < D[base7 + 2]) D[base7 + 2] = newPath2;
                    if (newPath3 < D[base7 + 3]) D[base7 + 3] = newPath3;
                    if (newPath4 < D[base7 + 4]) D[base7 + 4] = newPath4;
                    if (newPath5 < D[base7 + 5]) D[base7 + 5] = newPath5;
                    if (newPath6 < D[base7 + 6]) D[base7 + 6] = newPath6;
                    if (newPath7 < D[base7 + 7]) D[base7 + 7] = newPath7;
                }
            }

            for (; j < jEnd; ++j) {
                WeightType Dkj = D[kOffset + j];
                if (Dik0 < INF) {
                    WeightType newPath0 = Dik0 + Dkj;
                    if (newPath0 < D[iOffset0 + j]) D[iOffset0 + j] = newPath0;
                }
                if (Dik1 < INF) {
                    WeightType newPath1 = Dik1 + Dkj;
                    if (newPath1 < D[iOffset1 + j]) D[iOffset1 + j] = newPath1;
                }
                if (Dik2 < INF) {
                    WeightType newPath2 = Dik2 + Dkj;
                    if (newPath2 < D[iOffset2 + j]) D[iOffset2 + j] = newPath2;
                }
                if (Dik3 < INF) {
                    WeightType newPath3 = Dik3 + Dkj;
                    if (newPath3 < D[iOffset3 + j]) D[iOffset3 + j] = newPath3;
                }
                if (Dik4 < INF) {
                    WeightType newPath4 = Dik4 + Dkj;
                    if (newPath4 < D[iOffset4 + j]) D[iOffset4 + j] = newPath4;
                }
                if (Dik5 < INF) {
                    WeightType newPath5 = Dik5 + Dkj;
                    if (newPath5 < D[iOffset5 + j]) D[iOffset5 + j] = newPath5;
                }
                if (Dik6 < INF) {
                    WeightType newPath6 = Dik6 + Dkj;
                    if (newPath6 < D[iOffset6 + j]) D[iOffset6 + j] = newPath6;
                }
                if (Dik7 < INF) {
                    WeightType newPath7 = Dik7 + Dkj;
                    if (newPath7 < D[iOffset7 + j]) D[iOffset7 + j] = newPath7;
                }
            }
        }

        for (; i < iEnd; ++i) {
            const int iOffset = i * n;
            const WeightType Dik = D[iOffset + k];

            if (Dik >= INF) {
                continue;
            }

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
                if (newPath0 < D[base]) D[base] = newPath0;
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

void fwBlockingUnroll(WeightType* D, int n, int blockSize)
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
        computeFWBlockUnroll(D, n, kStart, kStart, kStart, kEnd, kEnd, kEnd);

        for (int jBlock = 0; jBlock < numBlocks; ++jBlock) {
            if (jBlock == kBlock) continue;
            const int jStart = jBlock * blockSize;
            const int jEnd = std::min(jStart + blockSize, n);
            computeFWBlockUnroll(D, n, kStart, jStart, kStart, kEnd, jEnd, kEnd);
        }

        for (int iBlock = 0; iBlock < numBlocks; ++iBlock) {
            if (iBlock == kBlock) continue;
            const int iStart = iBlock * blockSize;
            const int iEnd = std::min(iStart + blockSize, n);
            computeFWBlockUnroll(D, n, iStart, kStart, kStart, iEnd, kEnd, kEnd);
        }

        for (int iBlock = 0; iBlock < numBlocks; ++iBlock) {
            if (iBlock == kBlock) continue;
            const int iStart = iBlock * blockSize;
            const int iEnd = std::min(iStart + blockSize, n);
            for (int jBlock = 0; jBlock < numBlocks; ++jBlock) {
                if (jBlock == kBlock) continue;
                const int jStart = jBlock * blockSize;
                const int jEnd = std::min(jStart + blockSize, n);
                computeFWBlockUnroll(D, n, iStart, jStart, kStart, iEnd, jEnd, kEnd);
            }
        }
    }
}