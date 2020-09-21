#include <mln/core/extension/padding.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include <fixtures/ImageCompare/image_compare.hpp>

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
  mln_foreach(auto p, f.domain())
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


template <class T, int dim>
void check_padding(mln::__ndbuffer_image<T, dim> input, mln::__ndbuffer_image<T, dim> tile, mln::e_padding_mode bm, T value)
{
  auto D = input.domain();


  mln_foreach(auto p, tile.domain())
  {
    T val;
    if (D.has(p))
      val = input(p);
    else
    {
      switch (bm)
      {
      case mln::PAD_ZERO: val = 0; break;
      case mln::PAD_CONSTANT: val = value; break;
      case mln::PAD_WRAP: val = input(D.periodize(p)); break;
      case mln::PAD_MIRROR: val = input(D.mirror(p)); break;
      case mln::PAD_REPLICATE: val = input(D.clamp(p)); break;
      }
    }
    ASSERT_EQ(tile(p), val);
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
public:
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
INSTANTIATE_TEST_SUITE_P(bla, Padding2DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_SUITE_P(bla, Padding2DTestRGB, testing::ValuesIn(paddings));


template <class T>
class CopyPadding2DTest : public testing::TestWithParam<mln::e_padding_mode>
{
  mln::image2d<T> input = {
    {  1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15}, //
    { 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30}, //
    { 31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45}, //
    { 46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60}, //
    { 61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75}, //
    { 76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90}, //
    { 91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104, 105}, //
    {106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120}, //
    {121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135}, //
    {136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150}, //
  };

  mln::image2d<T> r0_refs[5] = {
    {
      {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, //
      {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, //
      {  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10}, //
      {  0,  0,  0, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}, //
      {  0,  0,  0, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40}, //
      {  0,  0,  0, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55}, //
      {  0,  0,  0, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70}, //
    },
    {
      { 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69}, //
      { 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69}, //
      { 69, 69, 69,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10}, //
      { 69, 69, 69, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}, //
      { 69, 69, 69, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40}, //
      { 69, 69, 69, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55}, //
      { 69, 69, 69, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70}, //
    },
    {
      { 133, 134, 135, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130}, //
      { 148, 149, 150, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145}, //
      {  13,  14,  15,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10}, //
      {  28,  29,  30,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25}, //
      {  43,  44,  45,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40}, //
      {  58,  59,  60,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55}, //
      {  73,  74,  75,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70}, //
    },
    {
      { 18, 17, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}, //
      {  3,  2,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10}, //
      {  3,  2,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10}, //
      { 18, 17, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}, //
      { 33, 32, 31, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40}, //
      { 48, 47, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55}, //
      { 63, 62, 61, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70}, //
    },
    {
      {  1,  1,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10}, //
      {  1,  1,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10}, //
      {  1,  1,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10}, //
      { 16, 16, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}, //
      { 31, 31, 31, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40}, //
      { 46, 46, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55}, //
      { 61, 61, 61, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70}, //
    }
  };

  mln::image2d<T> r1_refs[5] = {
    {
      {118, 119, 120,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //
      {133, 134, 135,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //
      {148, 149, 150,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //
      {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //
      {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //
      {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, //
      {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0} //
    },{ //
      {118, 119, 120,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69}, //
      {133, 134, 135,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69}, //
      {148, 149, 150,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69}, //
      { 69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69}, //
      { 69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69}, //
      { 69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69}, //
      { 69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69}, //
    },{ //
      {118, 119, 120, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115}, //
      {133, 134, 135, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130}, //
      {148, 149, 150, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145}, //
      { 13,  14,  15,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10}, //
      { 28,  29,  30,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25}, //
      { 43,  44,  45,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40}, //
      { 58,  59,  60,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55}, //
    },{ //
      {118, 119, 120, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111}, //
      {133, 134, 135, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126}, //
      {148, 149, 150, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141}, //
      {148, 149, 150, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141}, //
      {133, 134, 135, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126}, //
      {118, 119, 120, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111}, //
      {103, 104, 105, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96}, //
    },{ //
      {118, 119, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120}, //
      {133, 134, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135}, //
      {148, 149, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150}, //
      {148, 149, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150}, //
      {148, 149, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150}, //
      {148, 149, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150}, //
      {148, 149, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150}, //
    }};

public:
  void test_copy_upper_left()
    {
      mln::image2d<T> refs[5];
      refs[mln::PAD_ZERO] = r0_refs[0];
      refs[mln::PAD_CONSTANT] = r0_refs[1];
      refs[mln::PAD_WRAP] = r0_refs[2];
      refs[mln::PAD_MIRROR] = r0_refs[3];
      refs[mln::PAD_REPLICATE] = r0_refs[4];

      auto bm = GetParam();
      T    v  = 69;
      int  w  = 13;
      int  h  = 7;

      mln::box2d      roi(-3, -2, w, h);
      mln::image2d<T> out(roi);
      copy_pad(input, out, bm, v);
      check_padding(input, out, bm, v);
      ASSERT_IMAGES_EQ_EXP(out, refs[bm]);
    }

    void test_copy_lower_right()
    {
      mln::image2d<T> refs[5];
      refs[mln::PAD_ZERO] = r1_refs[0];
      refs[mln::PAD_CONSTANT] = r1_refs[1];
      refs[mln::PAD_WRAP] = r1_refs[2];
      refs[mln::PAD_MIRROR] = r1_refs[3];
      refs[mln::PAD_REPLICATE] = r1_refs[4];

      auto bm = GetParam();
      T    v  = 69;
      int  w  = 13;
      int  h  = 7;

      mln::box2d      roi(12, 7, w, h);
      mln::image2d<T> out(roi);
      copy_pad(input, out, bm, v);
      check_padding(input, out, bm, v);
      ASSERT_IMAGES_EQ_EXP(out, refs[bm]);
    }
};




using CopyPadding2DTestInt = CopyPadding2DTest<int>;
using CopyPadding2DTestRGB = CopyPadding2DTest<rgb_t>;
TEST_P(CopyPadding2DTestInt, UpperLeft) {this->test_copy_upper_left();}
TEST_P(CopyPadding2DTestInt, LowerRight) {this->test_copy_lower_right();}
TEST_P(CopyPadding2DTestRGB, UpperLeft) {this->test_copy_upper_left();}
TEST_P(CopyPadding2DTestRGB, LowerRight) {this->test_copy_lower_right();}


INSTANTIATE_TEST_SUITE_P(bla, CopyPadding2DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_SUITE_P(bla, CopyPadding2DTestRGB, testing::ValuesIn(paddings));



template <class T>
class Padding1DTest : public testing::TestWithParam<mln::e_padding_mode>
{
  mln::image1d<T> input = {0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0};

public:
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

INSTANTIATE_TEST_SUITE_P(bla, Padding1DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_SUITE_P(bla, Padding1DTestRGB, testing::ValuesIn(paddings));


template <class T>
class Padding3DTest : public testing::TestWithParam<mln::e_padding_mode>
{
  mln::image3d<T> input;

public:
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



INSTANTIATE_TEST_SUITE_P(bla, Padding3DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_SUITE_P(bla, Padding3DTestRGB, testing::ValuesIn(paddings));



template <class T>
class CopyPadding3DTest : public testing::TestWithParam<mln::e_padding_mode>
{
  mln::image3d<T> input;

public:
  CopyPadding3DTest()
    : input(15, 9, 4)
  {
    mln::iota(input, 1);
  }

  void test_upper_left()
  {
    mln::box3d      roi(-3, -2, -1, 10, 6, 6);
    mln::image3d<T> out(roi);

    auto      bm           = GetParam();
    T         v            = 69;
    copy_pad(input, out, bm, v);
    check_padding(input, out, bm, v);
  }


  void test_lower_right()
  {
    mln::box3d      roi(7, 3, 3, 10, 6, 6);
    mln::image3d<T> out(roi);

    auto      bm           = GetParam();
    T         v            = 69;
    copy_pad(input, out, bm, v);
    check_padding(input, out, bm, v);
  }



};

using CopyPadding3DTestInt = CopyPadding3DTest<int>;
using CopyPadding3DTestRGB = CopyPadding3DTest<rgb_t>;

TEST_P(CopyPadding3DTestInt, UpperLeft) { this->test_upper_left(); }
TEST_P(CopyPadding3DTestInt, LowerRight) { this->test_lower_right(); }
TEST_P(CopyPadding3DTestRGB, UpperLeft) { this->test_upper_left(); }
TEST_P(CopyPadding3DTestRGB, LowerRight) { this->test_lower_right(); }



INSTANTIATE_TEST_SUITE_P(bla, CopyPadding3DTestInt, testing::ValuesIn(paddings));
INSTANTIATE_TEST_SUITE_P(bla, CopyPadding3DTestRGB, testing::ValuesIn(paddings));

