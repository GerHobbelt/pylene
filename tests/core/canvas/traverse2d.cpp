#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/canvas/private/traverse2d.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>


#include <gtest/gtest.h>
#include <mln/io/imprint.hpp>
#include <fmt/format.h>

namespace
{
  void fill_line(mln::image2d<int>& input,
                 mln::point2d direction,
                 mln::point2d p, std::size_t n)
  {
    for (std::size_t i = 0; i < n; ++i, p += direction)
      input(p) += 1;
  }
} // namespace

void test_direction(mln::point2d dir)
{
  using namespace mln::view::ops;

  mln::box2d roi(20, 10);

  mln::image_build_params params;
  params.init_value = 0;

  mln::image2d<int> input(roi, params);


  mln::canvas::details::traverse_along_direction(
    roi, dir, [=, &input](mln::point2d p, mln::point2d dir, std::size_t n) { fill_line(input, dir, p, n); });

  EXPECT_TRUE(mln::all_of(input == 1)) << fmt::format("with direction ({},{})", dir.x(), dir.y());
}


TEST(Dilation, Traverse)
{
  // Positive offsets
  test_direction({+0, +1});
  test_direction({+1, +0});
  test_direction({+1, +3});
  test_direction({+3, +1});
  test_direction({-1, -3});
  test_direction({-3, -1});

  // Negative
  test_direction({+0, -1});
  test_direction({-1, +0});
  test_direction({+1, -3});
  test_direction({+3, -1});
  test_direction({-1, +3});
  test_direction({-3, +1});




}
