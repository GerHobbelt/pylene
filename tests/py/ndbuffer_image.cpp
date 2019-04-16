#include <py/core/ndbuffer_image.hpp>
#include <gtest/gtest.h>

TEST(Py, ndbuffer_default_constructor)
{
  mln::ndbuffer_image a;
  EXPECT_EQ(0, a.width());
  EXPECT_EQ(0, a.height());
  EXPECT_EQ(0, a.depth());
  EXPECT_EQ(mln::sample_type_id::OTHER, a.sample_type());
  EXPECT_EQ(0, a.border());
  EXPECT_EQ(0, a.pdim());
}

static int kWidth = 10;
static int kHeight = 15;
static int kDepth = 9;
static int kDuration = 11;

static int kXmin = -5;
static int kXmax = +5;
static int kYmin = -7;
static int kYmax = +8;
static int kZmin = -18;
static int kZmax = -9;
static int kTmin = 0;
static int kTmax = 11;



void iota(mln::ndbuffer_image& a)
{
  int border = a.border();
  int i = 0;
  for (int w = -border; w < kDuration + border; ++w)
    for (int z = -border; z < kDepth + border; ++z)
      for (int y = -border; y < kHeight + border; ++y)
      {
        std::uint32_t* lineptr = reinterpret_cast<std::uint32_t*>(a.buffer()) + y * a.stride(1) + z * a.stride(2) + w * a.stride(3);
        for (int x = -border; x < kWidth + border; ++x)
          lineptr[x] = i++;
      }
}


void check_1d(mln::ndbuffer_image& a, int border)
{
  EXPECT_EQ(kWidth, a.width());
  EXPECT_EQ(1, a.height());
  EXPECT_EQ(1, a.depth());
  EXPECT_EQ(1, a.pdim());
  EXPECT_EQ(mln::sample_type_id::UINT32, a.sample_type());
  EXPECT_EQ(border, a.border());
  EXPECT_NE(nullptr, a.buffer());
  EXPECT_EQ(1, a.stride(0));
  EXPECT_EQ(4, a.byte_stride(0));
  EXPECT_EQ(0, a.stride(1));
  EXPECT_EQ(0, a.byte_stride(1));
  EXPECT_EQ(0, a.stride(2));
  EXPECT_EQ(0, a.byte_stride(2));
  iota(a);
}

TEST(Py, ndbuffer_constructor_1d)
{
  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(kWidth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(mln::Point{kXmin}, mln::Point{kXmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(kWidth);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth, bp);
    check_1d(a, 0);
  }
  {
    mln::Box domain(kWidth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_1d(a, 0);
  }
  {
    mln::Box domain(mln::Point{kXmin}, mln::Point{kXmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_1d(a, 0);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, bp);
    check_1d(a, 0);
  }
  {
    mln::Box domain(kWidth);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_1d(a, 0);
  }
}

void check_2d(mln::ndbuffer_image& a, int border)
{
  EXPECT_EQ(kWidth, a.width());
  EXPECT_EQ(kHeight, a.height());
  EXPECT_EQ(1, a.depth());
  EXPECT_EQ(2, a.pdim());
  EXPECT_EQ(mln::sample_type_id::UINT32, a.sample_type());
  EXPECT_EQ(border, a.border());
  EXPECT_NE(nullptr, a.buffer());
  EXPECT_EQ(1, a.stride(0));
  EXPECT_EQ(4, a.byte_stride(0));
  EXPECT_EQ(kWidth + 2 * border, a.stride(1));
  EXPECT_EQ(4 * (kWidth + 2 * border), a.byte_stride(1));
  EXPECT_EQ(0, a.stride(2));
  EXPECT_EQ(0, a.byte_stride(2));
  iota(a);
}

TEST(Py, ndbuffer_constructor_2d)
{
  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth, kHeight);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(kWidth, kHeight);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(mln::Point{kXmin,kYmin}, mln::Point{kXmax,kYmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(kWidth, kHeight);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth, kHeight, bp);
    check_2d(a, 0);
  }
  {
    mln::Box domain(kWidth, kHeight);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_2d(a, 0);
  }
  {
    mln::Box domain(mln::Point{kXmin,kYmin}, mln::Point{kXmax,kYmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_2d(a, 0);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight, bp);
    check_2d(a, 0);
  }
  {
    mln::Box domain(kWidth, kHeight);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_2d(a, 0);
  }
}

void check_3d(mln::ndbuffer_image& a, int border)
{
  EXPECT_EQ(kWidth, a.width());
  EXPECT_EQ(kHeight, a.height());
  EXPECT_EQ(kDepth, a.depth());
  EXPECT_EQ(3, a.pdim());
  EXPECT_EQ(mln::sample_type_id::UINT32, a.sample_type());
  EXPECT_EQ(border, a.border());
  EXPECT_NE(nullptr, a.buffer());
  EXPECT_EQ(1, a.stride(0));
  EXPECT_EQ(4, a.byte_stride(0));
  EXPECT_EQ(kWidth + 2 * border, a.stride(1));
  EXPECT_EQ(4 * (kWidth + 2 * border), a.byte_stride(1));
  int tmp = (kHeight + 2 * border) * (kWidth + 2 * border);
  EXPECT_EQ(tmp, a.stride(2));
  EXPECT_EQ(4 * tmp, a.byte_stride(2));
  iota(a);
}

TEST(Py, ndbuffer_constructor_3d)
{
  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth, kHeight, kDepth);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(kWidth, kHeight, kDepth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(mln::Point{kXmin,kYmin,kZmin}, mln::Point{kXmax,kYmax,kZmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight, kDepth);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(kWidth, kHeight, kDepth);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth, kHeight, kDepth, bp);
    check_3d(a, 0);
  }
  {
    mln::Box domain(kWidth, kHeight, kDepth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_3d(a, 0);
  }
  {
    mln::Box domain(mln::Point{kXmin,kYmin,kZmin}, mln::Point{kXmax,kYmax,kZmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_3d(a, 0);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight, kDepth, bp);
    check_3d(a, 0);
  }
  {
    mln::Box domain(kWidth, kHeight, kDepth);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_3d(a, 0);
  }
}

void check_4d(mln::ndbuffer_image& a, int border)
{
  EXPECT_EQ(kWidth, a.width());
  EXPECT_EQ(kHeight, a.height());
  EXPECT_EQ(kDepth, a.depth());
  EXPECT_EQ(kDuration, a.size(3));
  EXPECT_EQ(4, a.pdim());
  EXPECT_EQ(mln::sample_type_id::UINT32, a.sample_type());
  EXPECT_EQ(border, a.border());
  EXPECT_NE(nullptr, a.buffer());

  int tmp = 1;
  EXPECT_EQ(tmp, a.stride(0));
  EXPECT_EQ(4 * tmp, a.byte_stride(0));

  tmp *= kWidth + 2 * border;
  EXPECT_EQ(tmp, a.stride(1));
  EXPECT_EQ(4 * tmp, a.byte_stride(1));
  tmp *= kHeight + 2 * border;
  EXPECT_EQ(tmp, a.stride(2));
  EXPECT_EQ(4 * tmp, a.byte_stride(2));
  tmp *= kDepth + 2 * border;
  EXPECT_EQ(tmp, a.stride(3));
  EXPECT_EQ(4 * tmp, a.byte_stride(3));
  iota(a);
}


TEST(Py, ndbuffer_constructor_4d)
{
  {
    mln::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(mln::Point{kXmin,kYmin,kZmin,kTmin}, mln::Point{kXmax,kYmax,kZmax,kTmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_4d(a, 0);
  }
  {
    mln::Box domain(mln::Point{kXmin,kYmin,kZmin,kTmin}, mln::Point{kXmax,kYmax,kZmax,kTmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_4d(a, 0);
  }
  {
    mln::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_4d(a, 0);
  }
}






