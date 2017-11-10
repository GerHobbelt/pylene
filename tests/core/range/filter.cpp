#include <mln/core/domain/box.hpp>
#include <mln/core/forall.hpp>
#include <mln/core/range/filter.hpp>

#include <iostream>

#include <gtest/gtest.h>

TEST(Core, range_filter)
{

    using namespace mln;

    box2d a({0, 0}, {6, 11});
    auto x = rng::filter(a, [](point2d p) { return (p[0] % 2) == (p[1] % 2); }); // chess board

    unsigned sz = 0;
    mln_foreach (point2d p, x)
    {
        (void)p;
        ++sz;
    }

    ASSERT_TRUE(x.has(point2d{0, 0}));
    ASSERT_TRUE(x.has(point2d{1, 1}));
    ASSERT_TRUE(not x.has(point2d{0, 1}));
    ASSERT_TRUE(not x.has(point2d{1, 0}));
    ASSERT_EQ(sz, 33);
}
