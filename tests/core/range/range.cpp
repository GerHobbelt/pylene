#include <mln/core/image/image.hpp>
#include <mln/core/range/range.hpp>

#include <array>

#include <gtest/gtest.h>

TEST(Core, stdrange_compatibility)
{

    using namespace mln;

    std::array<int, 4> x = {{2, 5, 15, 22}};

    ASSERT_EQ(rng::size(x), 4);

    {
        int i = 0;
        mln_foreach (int v, x)
            ASSERT_EQ(v, x[i++]);
    }

    {
        int i = 0;
        mln_iter(pv, x);
        mln_forall (pv)
            ASSERT_EQ(*pv, x[i++]);
    }
}
