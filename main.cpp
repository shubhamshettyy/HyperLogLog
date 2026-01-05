/**
 * @file main.cpp
 * @brief Test harness for HyperLogLog
 */

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <set>
#include <iomanip>
#include <cmath>

#include "hyperloglog.h"

// Helper to generate random strings
std::string generate_random_string(size_t length) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string s;
    s.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        s += charset[dist(rng)];
    }
    return s;
}

// The Experiment Function
void run_experiment(int b, int num_elements) {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Running Experiment: b=" << b 
              << " (Registers: " << (1 << b) << "), " 
              << "Items=" << num_elements << "\n";

    // Initialize HLL and Ground Truth (Set)
    HyperLogLog hll(b);
    std::set<std::string> ground_truth;

    // Generate Data
    // We intentionally generate slightly fewer unique items than iterations
    // to ensure we have some duplicates (collisions) to test the robustness.
    for (int i = 0; i < num_elements; ++i) {
        // Generate a random string of length 10
        std::string val = generate_random_string(10);
        
        hll.AddElem(val);
        ground_truth.insert(val);
    }

    // Get Results
    int estimated_cardinality = hll.ComputeCardinality();
    int actual_cardinality = ground_truth.size();

    // Calculate Error
    double error_pct = (double)std::abs(estimated_cardinality - actual_cardinality) / actual_cardinality * 100.0;

    // Theoretical Error (Standard Error for HLL is 1.04 / sqrt(m))
    double expected_error = (1.04 / std::sqrt(1 << b)) * 100.0;

    // Output
    std::cout << "Actual Unique Count:    " << actual_cardinality << "\n";
    std::cout << "HLL Estimated Count:    " << estimated_cardinality << "\n";
    std::cout << "Relative Error:         " << std::fixed << std::setprecision(4) << error_pct << "%\n";
    std::cout << "Theoretical Error Limit:" << expected_error << "%\n";
    
    if (error_pct < expected_error * 3) {
         std::cout << "Result: [PASS] Error is within acceptable statistical bounds.\n";
    } else {
         std::cout << "Result: [FAIL/WARN] Error is unusually high.\n";
    }
    std::cout << "--------------------------------------------------\n\n";
}

int main() {
    run_experiment(6, 10000);

    run_experiment(10, 50000);

    run_experiment(14, 200000);

    return 0;
}