#include <mln/core/extension/padding.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/io/imprint.hpp>
#include <gtest/gtest.h>


struct rgb_t
{
  rgb_t() = default;
  rgb_t(uint8_t x)
    : r{x}
    , g{x}
    , b{x}
  {
  }

  bool operator==(rgb_t o) const { return r == o.r && g == o.g && b == o.b; }
  bool operator!=(rgb_t o) const { return !(*this == o); }

  uint8_t r,g,b;
};

template <class T>
void check_padding(mln::image2d<T> f, mln::box2d inner,  mln::e_padding_mode bm, T value)
{
  mln_foreach_new(auto p, f.domain())
  {
    if (inner.has(p))
      continue;

    T val;
    switch (bm)
    {
    case mln::PAD_ZERO: val = 0; break;
    case mln::PAD_CONSTANT: val = value; break;
    case mln::PAD_WRAP: val = f(inner.periodize(p)); break;
    case mln::PAD_MIRROR: val = f(inner.mirror(p)); break;
    case mln::PAD_REPLICATE: val = f(inner.clamp(p)); break;
    }
    ASSERT_EQ(f(p), val) << fmt::format("p = ({},{})", p.x(), p.y());
  }
}


template <class T>
class Padding2DTest : public testing::TestWithParam<mln::e_padding_mode>
{
  mln::image2d<T> input = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
  };

  void test_image()
  {
    auto      bm           = GetParam();
    const int borders[][2] = {{4, 8}, {3, 4}};
    T         v            = 69;
    pad(input, bm, borders, v);
    // mln::io::imprint(input);
    check_padding(input, mln::box2d(4, 3, 3, 2), bm, v);
  }

  void test_span()
  {
    auto      bm           = GetParam();
    const int borders[][2] = {{4, 8}, {3, 4}};
    T         v            = 69;
    pad(input.values(), bm, borders, v);
    check_padding(input, mln::box2d(4, 3, 3, 2), bm, v);
  }
};

using Padding2DTestInt = Padding2DTest<int>;
using Padding2DTestRGB = Padding2DTest<rgb_t>;

TEST_P(Padding2DTestInt, Image) { this->test_image(); }
TEST_P(Padding2DTestInt, Span) { this->test_span(); }
TEST_P(Padding2DTestRGB, Image) { this->test_image(); }
TEST_P(Padding2DTestRGB, Span) { this->test_span(); }



mln::e_padding_mode paddings[] = {mln::PAD_ZERO, mln::PAD_CONSTANT, mln::PAD_WRAP, mln::PAD_MIRROR, mln::PAD_REPLICATE};

INSTANTIATE_TEST_CASE_P(bla, Padding2DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_CASE_P(bla, Padding2DTestRGB, testing::ValuesIn(paddings));
