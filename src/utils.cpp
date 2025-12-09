#include "utils.h"
#include "fw_baseline.h"
#include "fw_baseline_unroll.h"
#include "fw_baseline_simd.h"
#include "fw_baseline_parallel.h"
#include "fw_baseline_parallel_simd.h"
#include "fw_blocking_baseline.h"
#include "fw_blocking_unroll.h"
#include "fw_blocking_simd.h"
#include "fw_blocking_parallel.h"
#include "fw_blocking_parallel_simd.h"
#include "fw_blocking_generalized_baseline.h"
#include "fw_blocking_generalized_unroll.h"
#include "fw_blocking_generalized_simd.h"
#include "fw_blocking_generalized_parallel.h"
#include "fw_blocking_generalized_parallel_simd.h"
#include <stdexcept>

bool hasNegativeCycle(const WeightType* distMatrix, int n) {
	for (int i = 0; i < n; ++i) {
		if (distMatrix[i * n + i] < 0) {
			return true;
		}
	}
	return false;
}

const std::map<std::string, AlgorithmFunc> ALGORITHM_MAP = {
	{"baseline", fwBaseline},
	{"baseline_unroll", fwBaselineUnroll},
	{"baseline_simd", fwBaselineSimd},
	{"baseline_parallel", fwBaselineParallel},
	{"baseline_parallel_simd", fwBaselineParallelSimd},

	{"blocking_baseline", fwBlockingBaseline},
	{"blocking_unroll", fwBlockingUnroll},
	{"blocking_simd", fwBlockingSimd},
	{"blocking_parallel", fwBlockingParallel},
	{"blocking_parallel_simd", fwBlockingParallelSimd},

	{"blocking_generalized_baseline", fwBlockingGeneralizedBaseline},
	{"blocking_generalized_unroll", fwBlockingGeneralizedUnroll},
	{"blocking_generalized_simd", fwBlockingGeneralizedSimd},
	{"blocking_generalized_parallel", fwBlockingGeneralizedParallel},
	{"blocking_generalized_parallel_simd", fwBlockingGeneralizedParallelSimd},
};

AlgorithmFunc getAlgorithmFunc(const std::string& algorithmName) {
	auto it = ALGORITHM_MAP.find(algorithmName);

	if (it == ALGORITHM_MAP.end()) {
		throw std::out_of_range("ERROR: Unsupported algorithm name provided: " + algorithmName);
	}

	return it->second;
}