#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>

#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

TEST(Algorithm, Copy)
{
    using namespace mln;

    image2d<uint8> ima(10, 10);
    image2d<uint8> out(10, 10);
    iota(ima, 0);
    copy(ima, out);

    ASSERT_TRUE(equal(ima, out));
}
