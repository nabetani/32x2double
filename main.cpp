#include "d2_32x2.h"
#include <bit>
#include <format>
#include <iostream>

void test_one(double x)
{
    auto [i, ok_a] = d2_32x2(x);
    auto a = std::bit_cast<std::int64_t>(i);
    auto e = static_cast<std::int64_t>(x);
    auto ok_e = std::abs(x) < static_cast<double>(1ull << 63);

    if (ok_a && ok_e)
    {
        if (a != e)
        {
            std::cout << std::format("x={} a={:x} e={:x}", x, a, e) << std::endl;
            return;
        }
    }
    if (ok_a != ok_e)
    {
        std::cout << std::format("x={} e={:x} ok_a={}  ok_e={}", x, e, ok_a, ok_e) << std::endl;
        return;
    }
    std::cout << std::format("okay: x={}", x) << std::endl;
}

void test(double x)
{
    test_one(x);
    test_one(-x);
}

int main()
{
    test(0.0);
    test(1);
    test(255);
    test(256);
    test(0xaa55);
    test(65535);
    test(65536);
    for (int i = 17; i < 63; ++i)
    {
       std::cout << std::format("***** i={}", i) << std::endl;
        test((1ull << i));
        test((1ull << i) - 1);
        test((1ull << i) + 1);
        test((1ull << i) + (1ull << 31));
        if (31<i){
            test((1ull << i) - (1ull << 31));
        }
    }
    return 0;
}