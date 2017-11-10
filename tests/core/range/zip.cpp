#include <mln/core/image/image.hpp>
#include <mln/core/range/zip.hpp>

#include <array>
#include <iostream>

#include <gtest/gtest.h>

TEST(Core, ziprange)
{

    using namespace mln;

    std::array<int, 4> x{{2, 5, 15, 22}};
    std::array<int, 4> y{{-2, -5, -15, -22}};

    ASSERT_EQ(rng::size(x), 4);

    auto z = rng::zip(x, y);

    {
        int i = 0;
        mln_foreach (auto v, z)
        {
            ASSERT_EQ(std::get<0>(v), x[i]);
            ASSERT_EQ(std::get<1>(v), y[i++]);
        }
    }

    {
        int i = 0;
        mln_iter(pv, z);
        mln_forall (pv)
        {
            ASSERT_EQ(std::get<0>(*pv), x[i]);
            ASSERT_EQ(std::get<1>(*pv), y[i++]);
        }
    }
}
