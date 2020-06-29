#include <mln/core/algorithm/accumulate.hpp>

#include <mln/accu/accumulators/max.hpp>
#include <mln/accu/accumulators/min.hpp>
#include <mln/accu/accumulators/sum.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/ndimage.hpp>

#include <gtest/gtest.h>


TEST(Core, Algorithm_Accumulate_1)
{
  mln::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);

  // Expected overflow
  {
    int res = mln::accumulate(ima, 0, std::plus<uint8_t>());
    ASSERT_EQ(res, ((99 * 100) / 2) % 256);
  }

  // No overflow
  {
    int res = mln::accumulate(ima, 0, std::plus<int>());
    ASSERT_EQ(res, ((99 * 100) / 2));
  }
}

TEST(Core, Algorithm_Accumulate_2)
{
  mln::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);

  // No overflow (uint8 + uint8 -> int)
  {
    int res = mln::accumulate(ima, mln::accu::features::sum<>());
    ASSERT_EQ(res, ((99 * 100) / 2));
  }
}

TEST(Core, Algorithm_Accumulate_3)
{
  mln::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);

  // No overflow (uint8 + uint8 -> int)
  {
    auto acc = mln::accumulate(ima, mln::accu::features::min<>() & mln::accu::features::max<>());
    ASSERT_EQ(mln::accu::extractor::min(acc), 0);
    ASSERT_EQ(mln::accu::extractor::max(acc), 99);
  }
}
