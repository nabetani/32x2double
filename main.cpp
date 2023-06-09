#include "d2_32x2.h"
#include <bit>
#include <format>
#include <iostream>
#include <limits>
#include <cmath>

int test_one(double x)
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
            return 1;
        }
    }
    if (ok_a != ok_e)
    {
        std::cout << std::format("x={} e={:x} ok_a={}  ok_e={}", x, e, ok_a, ok_e) << std::endl;
        return 1;
    }
    std::cout << std::format("okay: x={}", x) << std::endl;
    return 0;
}

int test(double x)
{
    return test_one(x) + test_one(-x);
}

int testd(double x)
{
    auto n = std::nextafter(x, x * 2);
    auto p = std::nextafter(x, x / 2);
    return test(x) + test(n) + test(p);
}

int main()
{
    int failed = 0;
    failed += test(0.0);
    failed += test(1);
    failed += test(255);
    failed += test(256);
    failed += test(0xaa55);
    failed += test(0x1234'5678'abcd'2468);
    failed += test(65535);
    failed += test(65536);
    for (int i = 17; i < 63; ++i)
    {
        std::cout << std::format("***** i={}", i) << std::endl;
        failed += test((1ull << i));
        failed += test((1ull << i) - 1);
        failed += test((1ull << i) + 1);
        failed += test((1ull << i) + (1ull << 31));
        if (31 < i)
        {
            failed += test((1ull << i) - (1ull << 31));
        }
    }
    constexpr auto p62 = static_cast<double>(1ull << 62);
    failed += testd(p62);
    failed += testd(p62 * 2);
    failed += testd(p62 * 4);
    failed += testd(p62 * 8);
    failed += testd(p62 * 16);
    failed += testd(p62 * 32);

    using lim = std::numeric_limits<double>;
    failed += testd(lim::infinity());
    failed += testd(lim::quiet_NaN());
    failed += testd(lim::signaling_NaN());
    failed += testd(lim::denorm_min());

    std::cout << std::format("failed={}", failed) << std::endl;

    return 0;
}