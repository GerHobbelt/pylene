#include <mln/accu/accumulators/max.hpp>
#include <mln/accu/accumulators/min.hpp>
#include <mln/accu/accumulators/sum.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>

#include <gtest/gtest.h>

TEST(Algorithm, Accumulate_1)
{
    using namespace mln;

    image2d<uint8> ima(10, 10);
    iota(ima, 0);

    // Expected overflow
    {
        int res = accumulate(ima, std::plus<uint8>(), 0);
        ASSERT_EQ(res, ((99 * 100) / 2) % 256);
    }

    // No overflow
    {
        int res = accumulate(ima, std::plus<int>(), 0);
        ASSERT_EQ(res, ((99 * 100) / 2));
    }
}

TEST(Algorithm, Accumulate_2)
{
    using namespace mln;

    image2d<uint8> ima(10, 10);
    iota(ima, 0);

    // No overflow (uint8 + uint8 -> int)
    {
        int res = accumulate(ima, accu::features::sum<>());
        ASSERT_EQ(res, ((99 * 100) / 2));
    }
}

TEST(Algorithm, Accumulate_3)
{
    using namespace mln;

    image2d<uint8> ima(10, 10);
    iota(ima, 0);

    // No overflow (uint8 + uint8 -> int)
    {
        auto acc = accumulate(ima, accu::features::min<>() & accu::features::max<>());
        ASSERT_EQ(accu::extractor::min(acc), 0);
        ASSERT_EQ(accu::extractor::max(acc), 99);
    }
}
