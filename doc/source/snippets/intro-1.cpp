
// #BEGIN-INCLUDES
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/area_filter.hpp>
#include <mln/morpho/watershed.hpp>

#include <fmt/core.h>
#include "lut.hpp"
// #END-INCLUDES

// #BEGIN-CODE-2
template <class I, class N>
requires (mln::concepts::Image<I> &&
          mln::concepts::Neighborhood<N, mln::image_point_t<I>> &&
          std::unsigned_integral<mln::image_value_t<I>>)
auto chain_1(I input, N nbh, int lambda)
{
  using V = mln::image_value_t<I>;

  int nlabel = -1;
  auto negate = mln::transform(input, [](V x) -> V { return std::numeric_limits<V>::max() - x; });
  auto c = mln::morpho::area_closing(negate, nbh, lambda);
  auto s = mln::morpho::watershed<int8_t>(c, nbh, nlabel);
  return mln::transform(s, regions_lut);
}
// #END-CODE-2

// #BEGIN-CODE-3
#include <mln/core/image/view/transform.hpp>

template <class I, class N>
requires (mln::concepts::Image<I> &&
          mln::concepts::Neighborhood<N, mln::image_point_t<I>> &&
          std::unsigned_integral<mln::image_value_t<I>>)
auto chain_2(I input, N nbh, int lambda)
{
  using V = mln::image_value_t<I>;

  int nlabel = -1;
  auto negate = mln::view::transform(input, [](V x) -> V { return std::numeric_limits<V>::max() - x; });
  auto c = mln::morpho::area_closing(negate, nbh, lambda);
  auto s = mln::morpho::watershed<int8_t>(c, nbh, nlabel);
  return mln::transform(s, regions_lut);
}
// #END-CODE-3


int main(int argc, char** argv)
{
  if (argc != 3) {
    fmt::print(stderr, "Usage: {} input output\n", argv[0]); std::exit(1);
  }


  // #BEGIN-CODE
  mln::image2d<uint8_t> input;
  mln::io::imread(argv[1], input);
  int nlabel = -1;
  auto negate = mln::transform(input, [](uint8_t x) -> uint8_t { return 255 - x; });
  auto c = mln::morpho::area_closing(negate, mln::c4, 400);
  auto s = mln::morpho::watershed<int8_t>(c, mln::c4, nlabel);
  auto out = mln::transform(s, regions_lut);
  mln::io::imsave(out, argv[2]);
  fmt::print("Segmentation with {} regions.\n", nlabel);
  // #END-CODE

  {
    auto out = chain_1(input, mln::c4, 400);
  }

  {
    auto out = chain_2(input, mln::c4, 400);
  }


}
