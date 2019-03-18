#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>

#include <boost/range/algorithm/count.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Fill)
{
  using namespace mln;

  image2d<std::uint8_t> ima(10, 10);
  mln::fill(ima, 69);

  mln_viter(v, ima);
  mln_forall (v)
    ASSERT_TRUE(*v == 69);
}


TEST(Core, Algorithm_Exp_Fill)
{
  using namespace mln::experimental::ops;

  mln::image2d<std::uint8_t> ima(10, 10);
  mln::fill(ima, 69);

  ASSERT_TRUE(mln::all_of(ima == 69));
}
