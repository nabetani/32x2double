#pragma once

#include <cstdint>
#include <utility>
#include <bit>
#include <format>
#include <iostream>

struct large_int
{
    std::uint32_t lo_;
    std::int32_t hi_;
};

struct u32_pair
{
    std::uint32_t lo_, hi_;
    constexpr bool is_zero() const
    {
        return (hi_ & 0x7fff'ffff) == 0 && lo_ == 0;
    }
    constexpr int neg() const
    {
        return !!(hi_ & 0x8000'0000);
    }
    constexpr std::int32_t exp() const
    {
        return (hi_ >> 20) & 2047;
    }
    constexpr std::uint32_t hfrac() const
    {
        return (hi_ & 1048575) + (1u << 20);
    }
};

inline std::uint32_t shift(std::uint32_t v, int x)
{
    if (x <= -32)
    {
        return 0;
    }
    if (x < 0)
    {
        return v >> (-x);
    }
    if (x < 32)
    {
        return v << x;
    }
    return 0;
}

inline std::pair<large_int, bool> d2_32x2(double x)
{
    auto up = std::bit_cast<u32_pair>(x);
    if (up.is_zero())
    {
        return {{0, 0}, true};
    }
    auto f0 = up.hfrac();
    auto f1 = up.lo_;
    auto e = up.exp() + (-52 - 1023);
    large_int r = [&]()->large_int {
        if (0 <= e)
        {
            return {shift(f1, e),
                        std::int32_t(shift(f0, e) + shift(f1, e - 32))};
        }
        else
        {
            return {shift(f1, e) + shift(f0, e + 32),
                        std::int32_t(shift(f0, e))};
        }
    }();
    if (up.neg()){
        std::int32_t in = 0;
        if (r.lo_==0){
            return {{ 0, 1+~r.hi_ }, true };
        } else {
            return {{ 1+~r.lo_, ~r.hi_ }, true };
        }
    } else {
        return { r, true };
    }
}
