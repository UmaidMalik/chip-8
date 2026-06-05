#pragma once

#include <cstdint>
#include <random>

namespace rng
{
    inline uint8_t RandomNum_8Bit()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> distr(0, 255);

        return static_cast<uint8_t>(distr(gen));
    }
}