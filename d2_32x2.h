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
    constexpr bool is_like_zero() const
    {
        return exp()==0;
    }
    constexpr bool is_nan_inf() const
    {
        return exp()==0x7ff;
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
    if (up.is_like_zero())
    {
        return {{0, 0}, true};
    }
    if (up.is_nan_inf())
    {
        return {{}, false};
    }
    auto e = up.exp() + (-52 - 1023);
    if (10<e){
        return {{},false};
    }
    auto f0 = up.hfrac();
    auto f1 = up.lo_;
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
        if (r.lo_==0){
            return {{ 0, 1+~r.hi_ }, true };
        } else {
            return {{ 1+~r.lo_, ~r.hi_ }, true };
        }
    } else {
        return { r, true };
    }
}
