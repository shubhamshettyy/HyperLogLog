# C++ HyperLogLog Implementation

A header-only C++ implementation of the **HyperLogLog** probabilistic data structure for cardinality estimation (counting unique items in massive datasets).

## Overview
Counting unique items (cardinality) in a dataset typically requires $O(N)$ memory (e.g., using a HashSet). For billions of items, this becomes prohibitively expensive.

HyperLogLog (HLL) solves this by using **probabilistic counting**. It estimates the cardinality with a typical error of less than 2% using only **1.5 KB to 12 KB** of memory, regardless of the input size.

This project implements the HLL algorithm from scratch, including:
* **MurmurHash3** for high-quality bit distribution.
* **Stochastic Averaging** (Bucketing) to reduce variance.
* **Dynamic Alpha** constants for varying register sizes.

## Quick Start

### Prerequisites
* A C++ compiler supporting C++17 or later (e.g., `g++`, `clang++`).

### Installation
This is a **header-only** library. Simply include `hyperloglog.h` in your project.

```cpp
#include "hyperloglog.h"

int main() {
    // Initialize HLL with b=14 (16,384 registers)
    HyperLogLog hll(14); 

    hll.AddElem("user_123");
    hll.AddElem("user_456");
    hll.AddElem("user_123"); // Duplicate is ignored

    double count = hll.ComputeCardinality();
    std::cout << "Estimated Unique Items: " << count << std::endl;
    return 0;
}
```

## Build and Test
To run the included test harness (main.cpp) which generates random strings and compares HLL against a std::set:

```bash
# Compile
g++ main.cpp -o hll_test

# Run
./hll_test
```

## Performance & Accuracy
The implementation was stress-tested against a Ground Truth (`std::set`) using varying register sizes (`b`) and dataset sizes.

| Register Bits (`b`) | Registers ($2^b$) | Unique Items | Estimated | Relative Error | Theoretical Error Limit |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 6 | 64 | 10,000 | 9530 | **4.7%** | ~13.0% |
| 10 | 1,024 | 50,000 | 50,661 | **1.32%** | ~3.25% |
| 14 | 16,384 | 200,000 | 198,519 | **0.74%** | ~0.81% |
