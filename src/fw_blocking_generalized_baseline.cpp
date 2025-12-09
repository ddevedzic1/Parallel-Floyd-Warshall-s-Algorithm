#include "fw_blocking_generalized_baseline.h"
#include "constants.h"
#include <algorithm>
#include <iostream>

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

			if (Dik >= INF) {
				continue;
			}

			for (int j = jStart; j < jEnd; ++j) {
				const WeightType newPath = Dik + D[kOffset + j];
				WeightType& Dij = D[iOffset + j];
				if (newPath < Dij) {
					Dij = newPath;
				}
			}
		}
	}
}

static void computeTileDD(
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

	for (int i = iStart; i < iEnd; ++i) {
		const int iOffset = i * n;

		for (int j = jStart; j < jEnd; ++j) {
			WeightType Dij = D[iOffset + j];

			for (int k = kStart; k < kEnd; ++k) {
				const WeightType Dik = D[iOffset + k];
				if (Dik >= INF) {
					continue;
				}

				const WeightType newPath = Dik + D[k * n + j];
				if (newPath < Dij) {
					Dij = newPath;
				}
			}

			D[iOffset + j] = Dij;
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
			computeTileDD(D, n, kBlockBase + m, currentK, currentK, blockSize);
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
				computeTileDD(D, n, kBlockBase + m, currentK, jBlock, blockSize);
			}

			computeTile(D, n, currentK, currentK, jBlock, blockSize);
		}
	}

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
				computeTileDD(D, n, kBlockBase + m, iBlock, currentK, blockSize);
			}

			computeTile(D, n, currentK, iBlock, currentK, blockSize);
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

	for (int jBlock = 0; jBlock <= currentK; ++jBlock) {
		for (int m = l + 1; m < kappa; ++m) {
			computeTileDD(D, n, kBlockBase + m, currentK, jBlock, blockSize);
		}
	}

	for (int jBlock = kBlockBase + kappa; jBlock < numBlocks; ++jBlock) {
		for (int m = l + 1; m < kappa; ++m) {
			computeTileDD(D, n, kBlockBase + m, currentK, jBlock, blockSize);
		}
	}

	for (int iBlock = 0; iBlock < currentK; ++iBlock) {
		for (int m = l + 1; m < kappa; ++m) {
			computeTileDD(D, n, kBlockBase + m, iBlock, currentK, blockSize);
		}
	}

	for (int iBlock = kBlockBase + kappa; iBlock < numBlocks; ++iBlock) {
		for (int m = l + 1; m < kappa; ++m) {
			computeTileDD(D, n, kBlockBase + m, iBlock, currentK, blockSize);
		}
	}
}

static void calcRestBlocks(
	WeightType* D, int n,
	int kBlockBase, int kappa,
	int blockSize, int numBlocks)
{
	for (int iBlock = 0; iBlock < numBlocks; ++iBlock) {
		if (iBlock >= kBlockBase && iBlock < kBlockBase + kappa) {
			continue;
		}

		for (int jBlock = 0; jBlock < numBlocks; ++jBlock) {
			if (jBlock >= kBlockBase && jBlock < kBlockBase + kappa) {
				continue;
			}

			for (int m = 0; m < kappa; ++m) {
				computeTileDD(D, n, kBlockBase + m, iBlock, jBlock, blockSize);
			}
		}
	}
}

void fwBlockingGeneralizedBaseline(WeightType* D, int n, int blockSize)
{
	if (n <= 0 || n > MAX_N) {
		std::cerr << "ERROR: Graph size (n) must be between 1 and "
			<< MAX_N << ". Algorithm aborted.\n";
		return;
	}

	if (blockSize <= 0) {
		blockSize = n / 2;
	}
	if (blockSize < 1) blockSize = 1;

	const int kappa = 4;
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