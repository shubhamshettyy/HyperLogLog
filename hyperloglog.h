#include <iostream>
#include <vector>
#include <bitset>
#include <functional>
#include <algorithm>

class HyperLogLog
{
public:
    HyperLogLog(int b) : b_(b), m_(1 << b), registers_(m_, 0)
    {
        alpha_ = get_alpha(m_);
    }
    int get_b() { return b_; };
    int get_m() { return m_; };

    void AddElem(std::string val)
    {
        /*
        This method is the orchestrator
        1. hash value first
        2. get binary of hash
        3. get register index and p(rank)
        4. set register[index] = max(register[index], p)
        */
        uint64_t hash_value = CalculateHash(val);
        std::bitset<64> binary_hash = ComputeBinary(hash_value);
        int register_index = hash_value >> (64 - b_);
        int p = PositionOfLeftMostOne(binary_hash);
        registers_[register_index] = std::max(registers_[register_index], p);
    };

    int ComputeCardinality()
    {
        // calculate the cardinality using the formula
        double denominator = 0;
        for (int val : registers_)
        {
            denominator += (1.0 / (1 << val));
        }
        return std::floor(alpha_ * m_ * m_ / denominator);
    };

private:
    int b_; // no of bits for index
    int m_; // no of registers = 2^b
    double alpha_;
    std::vector<int> registers_;
    float alpha = 0.79402;

    // helpers

    double get_alpha(int m)
    {
        if (m == 16)
            return 0.673;
        if (m == 32)
            return 0.697;
        if (m == 64)
            return 0.709;
        // For m >= 128, use the formula
        return 0.7213 / (1.0 + 1.079 / m);
    }

    std::uint64_t CalculateHash(std::string val)
    {
        return MurmurHash64A(val.c_str(), val.length(), 0x9747b28c);
    }

    std::bitset<64> ComputeBinary(std::uint64_t hash)
    {
        // return the 64 bit binary stream
        std::bitset<64> binary_hash(hash);
        return binary_hash;
    }

    int PositionOfLeftMostOne(std::bitset<64> hash_value)
    {
        for (int i = 63 - b_; i >= 0; i--)
        {
            if (hash_value[i] == 1)
            {
                return (63 - b_ - i) + 1;
            }
        }
        return (64 - b_) + 1;
    }

    static uint64_t MurmurHash64A(const void *key, int len, unsigned int seed)
    {
        const uint64_t m = 0xc6a4a7935bd1e995;
        const int r = 47;

        uint64_t h = seed ^ (len * m);

        const uint64_t *data = (const uint64_t *)key;
        const uint64_t *end = data + (len / 8);

        while (data != end)
        {
            uint64_t k = *data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char *data2 = (const unsigned char *)data;

        switch (len & 7)
        {
        case 7:
            h ^= uint64_t(data2[6]) << 48;
        case 6:
            h ^= uint64_t(data2[5]) << 40;
        case 5:
            h ^= uint64_t(data2[4]) << 32;
        case 4:
            h ^= uint64_t(data2[3]) << 24;
        case 3:
            h ^= uint64_t(data2[2]) << 16;
        case 2:
            h ^= uint64_t(data2[1]) << 8;
        case 1:
            h ^= uint64_t(data2[0]);
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }
};
