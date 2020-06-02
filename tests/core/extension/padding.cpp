#include <mln/core/extension/padding.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/range/foreach.hpp>
#include <fmt/core.h>
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

template <class T, int dim>
void check_padding(mln::__ndbuffer_image<T, dim> f, mln::ndbox<dim> inner,  mln::e_padding_mode bm, T value)
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
    ASSERT_EQ(f(p), val);
  }
}


template <class T>
class Padding2DTest : public testing::TestWithParam<mln::e_padding_mode>
{
public:
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


template <class T>
class Padding1DTest : public testing::TestWithParam<mln::e_padding_mode>
{
  mln::image1d<T> input = {0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0};

  void test_image()
  {
    auto      bm           = GetParam();
    const int borders[][2] = {{4, 8}};
    T         v            = 69;
    pad(input, bm, borders, v);
    check_padding(input, mln::box1d(4, 3), bm, v);
  }

  void test_span()
  {
    auto      bm           = GetParam();
    const int borders[][2] = {{4, 8}};
    T         v            = 69;
    pad(input.values(), bm, borders, v);
    check_padding(input, mln::box1d(4, 3), bm, v);
  }
};

using Padding1DTestInt = Padding1DTest<int>;
using Padding1DTestRGB = Padding1DTest<rgb_t>;

TEST_P(Padding1DTestInt, Image) { this->test_image(); }
TEST_P(Padding1DTestInt, Span) { this->test_span(); }
TEST_P(Padding1DTestRGB, Image) { this->test_image(); }
TEST_P(Padding1DTestRGB, Span) { this->test_span(); }

INSTANTIATE_TEST_CASE_P(bla, Padding1DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_CASE_P(bla, Padding1DTestRGB, testing::ValuesIn(paddings));


template <class T>
class Padding3DTest : public testing::TestWithParam<mln::e_padding_mode>
{
  mln::image3d<T> input;

  Padding3DTest() : input(15, 9, 4)
    {
      input({4, 3, 1}) = 1;
      input({5, 3, 1}) = 2;
      input({6, 3, 1}) = 3;
      input({4, 4, 1}) = 4;
      input({5, 4, 1}) = 5;
      input({6, 4, 1}) = 6;
    }

  void test_image()
  {
    auto      bm           = GetParam();
    const int borders[][2] = {{4, 8}, {3, 4}, {1, 2}};
    T         v            = 69;
    pad(input, bm, borders, v);
    check_padding(input, mln::box3d(4, 3, 1, 3, 2, 1), bm, v);
  }

  void test_span()
  {
    auto      bm           = GetParam();
    const int borders[][2] = {{4, 8}, {3, 4}, {1, 2}};
    T         v            = 69;
    pad(input.values(), bm, borders, v);
    check_padding(input, mln::box3d(4, 3, 1, 3, 2, 1), bm, v);
  }
};

using Padding3DTestInt = Padding3DTest<int>;
using Padding3DTestRGB = Padding3DTest<rgb_t>;

TEST_P(Padding3DTestInt, Image) { this->test_image(); }
TEST_P(Padding3DTestInt, Span) { this->test_span(); }
TEST_P(Padding3DTestRGB, Image) { this->test_image(); }
TEST_P(Padding3DTestRGB, Span) { this->test_span(); }



INSTANTIATE_TEST_CASE_P(bla, Padding3DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_CASE_P(bla, Padding3DTestRGB, testing::ValuesIn(paddings));
