#include "tests.h"
#include "constants.h"
#include "utils.h"
#include "graph_generator.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

static int testsPassed = 0;
static int testsTotal = 0;

static void assertEqual(const std::vector<WeightType>& actual, const std::vector<WeightType>& expected, const std::string& testName) {
    testsTotal++;

    if (actual.size() != expected.size()) {
        std::cerr << "[ FAILED ] " << testName << ": Matrix size mismatch (Expected: " << expected.size()
            << ", Actual: " << actual.size() << ")\n";
        return;
    }

    int n = static_cast<int>(std::round(std::sqrt(actual.size())));

    for (int i = 0; i < static_cast<int>(actual.size()); ++i) {
        if (actual[i] != expected[i]) {
            std::cerr << "[ FAILED ] " << testName << ": Mismatch at position [" << i / n << "]["
                << i % n << "]. Expected: " << expected[i]
                << ", Actual: " << actual[i] << "\n";
            return;
        }
    }

    std::cout << "[ PASSED ] " << testName << "\n";
    testsPassed++;
}

static void runTest(AlgorithmFunc algorithm, const std::vector<WeightType>& input,
    const std::vector<WeightType>& expected, int n, int blockSize,
    const std::string& testName) {
    std::vector<WeightType> result = input;
    algorithm(result.data(), n, blockSize);
    assertEqual(result, expected, testName);
}

static void runTestAgainstReference(AlgorithmFunc algorithm, AlgorithmFunc reference,
    int n, int blockSize, double density, unsigned int seed,
    const std::string& testName) {
    std::vector<WeightType> input = generateGraph1D(n, density, seed);

    std::vector<WeightType> expected = input;
    reference(expected.data(), n, 0);

    std::vector<WeightType> result = input;
    algorithm(result.data(), n, blockSize);

    assertEqual(result, expected, testName);
}

// =============================================================================
// BASIC TESTS (Small Graphs, Manual Input)
// =============================================================================

static void testSingleNode(AlgorithmFunc algorithm, int blockSize) {
    std::vector<WeightType> input = { 0 };
    std::vector<WeightType> expected = { 0 };
    runTest(algorithm, input, expected, 1, blockSize, "Single node (1x1)");
}

static void testNoEdges(AlgorithmFunc algorithm, int blockSize) {
    std::vector<WeightType> input = {
        0, INF,
        INF, 0
    };
    runTest(algorithm, input, input, 2, blockSize, "No edges (2x2)");
}

static void testSimplePath(AlgorithmFunc algorithm, int blockSize) {
    std::vector<WeightType> input = {
        0, 1, INF, 100,
        INF, 0, 1, INF,
        INF, INF, 0, 1,
        INF, INF, INF, 0
    };
    std::vector<WeightType> expected = {
        0, 1, 2, 3,
        INF, 0, 1, 2,
        INF, INF, 0, 1,
        INF, INF, INF, 0
    };
    runTest(algorithm, input, expected, 4, blockSize, "Simple path optimization (4x4)");
}

static void testDisconnectedComponents(AlgorithmFunc algorithm, int blockSize) {
    std::vector<WeightType> input = {
        0, 1, INF, INF,
        1, 0, INF, INF,
        INF, INF, 0, 2,
        INF, INF, 2, 0
    };
    runTest(algorithm, input, input, 4, blockSize, "Disconnected components (4x4)");
}

static void testNegativeWeights(AlgorithmFunc algorithm, int blockSize) {
    std::vector<WeightType> input = {
        0, 5, 10,
        0, 0, -1,
        INF, INF, 0
    };
    std::vector<WeightType> expected = {
        0, 5, 4,
        0, 0, -1,
        INF, INF, 0
    };
    runTest(algorithm, input, expected, 3, blockSize, "Negative weights (3x3)");
}

static void testNegativeCycle(AlgorithmFunc algorithm, int blockSize) {
    testsTotal++;

    std::vector<WeightType> input = {
        0, 2, INF,
        INF, 0, 1,
        INF, -3, 0
    };

    std::vector<WeightType> result = input;
    algorithm(result.data(), 3, blockSize);

    if (hasNegativeCycle(result.data(), 3)) {
        std::cout << "[ PASSED ] Negative cycle detection (3x3)\n";
        testsPassed++;
    }
    else {
        std::cerr << "[ FAILED ] Negative cycle detection (3x3)\n";
    }
}

// =============================================================================
// LARGE GRAPH TESTS (Compare Against Reference)
// =============================================================================

static void testLargeGraph(AlgorithmFunc algorithm, AlgorithmFunc reference,
    int n, int blockSize, unsigned int seed) {
    std::string testName = "Large graph (N=" + std::to_string(n) + ", seed=" + std::to_string(seed) + ")";
    runTestAgainstReference(algorithm, reference, n, blockSize, 0.9, seed, testName);
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

void runTests(AlgorithmFunc algorithm, AlgorithmFunc reference,
    const std::string& name, int blockSize) {
    testsPassed = 0;
    testsTotal = 0;

    std::cout << "\n========================================\n";
    std::cout << "TESTING: " << name << " (blockSize=" << blockSize << ")\n";
    std::cout << "========================================\n";

    // Basic tests
    testSingleNode(algorithm, blockSize);
    testNoEdges(algorithm, blockSize);
    testSimplePath(algorithm, blockSize);
    testDisconnectedComponents(algorithm, blockSize);
    testNegativeWeights(algorithm, blockSize);
    testNegativeCycle(algorithm, blockSize);

    // Large graph tests (against reference)
    testLargeGraph(algorithm, reference, 128, blockSize, 42);
    testLargeGraph(algorithm, reference, 256, blockSize, 123);
    testLargeGraph(algorithm, reference, 512, blockSize, 456);
    testLargeGraph(algorithm, reference, 1024, blockSize, 789);

    std::cout << "========================================\n";
    std::cout << "RESULTS: " << testsPassed << "/" << testsTotal << " passed\n";
    std::cout << "========================================\n\n";
}