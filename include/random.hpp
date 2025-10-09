#pragma once

#include <random>

namespace rng
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(0, 255);

    uint8_t RandomNum_8Bit()
    {
        return distr(gen);
    }
}