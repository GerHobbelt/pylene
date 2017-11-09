#include <mln/core/algorithm/fill.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>

#include <boost/range/algorithm/count.hpp>

#include <gtest/gtest.h>

TEST(Algorithm, Fill)
{
    using namespace mln;

    image2d<uint8> ima(10, 10);
    fill(ima, 69);

    mln_viter(v, ima);
    mln_forall (v)
        ASSERT_TRUE(*v == 69);
}
