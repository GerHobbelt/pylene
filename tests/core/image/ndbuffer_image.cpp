#include <mln/core/image/experimental/ndbuffer_image.hpp>
#include <mln/core/image/experimental/ndimage.hpp>


#include <mln/core/concepts/image.hpp>
#include <mln/core/range/rows.hpp>

#include <numeric>
#include <cstdint>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>



static_assert(mln::concepts::RawImage<mln::experimental::image2d<int>>);


TEST(ndbuffer_image, default_constructor)
{
  mln::ndbuffer_image a;
  EXPECT_EQ(0, a.width());
  EXPECT_EQ(0, a.height());
  EXPECT_EQ(0, a.depth());
  EXPECT_EQ(mln::sample_type_id::OTHER, a.sample_type());
  EXPECT_EQ(0, a.border());
  EXPECT_EQ(0, a.pdim());
}

static mln::sample_type_id kSampleType = mln::sample_type_id::UINT32;
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
  assert(a.sample_type() == mln::sample_type_id::UINT32);

  int i = 0;
  for (int w = 0; w < a.size(3); ++w)
    for (int z = 0; z < a.depth(); ++z)
      for (int y = 0; y < a.height(); ++y)
      {
        std::uint32_t* lineptr = reinterpret_cast<std::uint32_t*>(a.buffer()) + y * a.stride(1) + z * a.stride(2) + w * a.stride(3);
        int width = a.width();
        for (int x = 0; x < width; ++x)
          lineptr[x] = i++;
      }

  EXPECT_EQ(i, a.domain().size());
}


enum
{
  MLN_COMPARE_DOMAIN = 0x01,
  MLN_COMPARE_EXTENSION = 0x02
};


/******************************************/
/****             Test 1D             ****/
/******************************************/


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

TEST(ndbuffer_image, constructors_1d)
{
  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(kWidth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(mln::experimental::Point{kXmin}, mln::experimental::Point{kXmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(kWidth);
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
    mln::experimental::Box domain(kWidth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_1d(a, 0);
  }
  {
    mln::experimental::Box domain(mln::experimental::Point{kXmin}, mln::experimental::Point{kXmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_1d(a, 0);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, bp);
    check_1d(a, 0);
  }
  {
    mln::experimental::Box domain(kWidth);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_1d(a, 0);
  }
}

TEST(image1d, constructors)
{
  {
    mln::experimental::image1d<uint32_t> a(kWidth);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box1d domain(kWidth);
    mln::experimental::image1d<uint32_t> a(domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box1d             domain(mln::experimental::point1d{kXmin}, mln::experimental::point1d{kXmax});
    mln::experimental::image1d<uint32_t> a(domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::image1d<uint32_t> a;
    a.resize(kWidth);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box1d domain(kWidth);
    mln::experimental::image1d<uint32_t> a;
    a.resize(domain);
    check_1d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::experimental::image1d<uint32_t> a(kWidth, bp);
    check_1d(a, 0);
  }
  {
    mln::experimental::box1d domain(kWidth);
    mln::experimental::image1d<uint32_t> a(domain, bp);
    check_1d(a, 0);
  }
  {
    mln::experimental::box1d             domain(mln::experimental::point1d{kXmin}, mln::experimental::point1d{kXmax});
    mln::experimental::image1d<uint32_t> a(domain, bp);
    check_1d(a, 0);
  }
  {
    mln::experimental::image1d<uint32_t> a;
    a.resize(kWidth, bp);
    check_1d(a, 0);
  }
  {
    mln::experimental::box1d domain(kWidth);
    mln::experimental::image1d<uint32_t> a;
    a.resize(domain, bp);
    check_1d(a, 0);
  }
}

/******************************************/
/****             Test 2D             ****/
/******************************************/


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

TEST(ndbuffer_image, constructors_2d)
{
  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth, kHeight);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain({kXmin, kYmin}, {kXmax, kYmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight);
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
    mln::experimental::Box domain(kWidth, kHeight);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_2d(a, 0);
  }
  {
    mln::experimental::Box domain({kXmin, kYmin}, {kXmax, kYmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_2d(a, 0);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight, bp);
    check_2d(a, 0);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_2d(a, 0);
  }
}

TEST(image2d, constructors)
{
  {
    mln::experimental::image2d<uint32_t> a(kWidth, kHeight);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box2d domain(kWidth, kHeight);
    mln::experimental::image2d<uint32_t> a(domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box2d domain({kXmin, kYmin}, {kXmax, kYmax});
    mln::experimental::image2d<uint32_t> a(domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::image2d<uint32_t> a;
    a.resize(kWidth, kHeight);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box2d domain(kWidth, kHeight);
    mln::experimental::image2d<uint32_t> a;
    a.resize(domain);
    check_2d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::experimental::image2d<uint32_t> a(kWidth, kHeight, bp);
    check_2d(a, 0);
  }
  {
    mln::experimental::box2d domain(kWidth, kHeight);
    mln::experimental::image2d<uint32_t> a(domain, bp);
    check_2d(a, 0);
  }
  {
    mln::experimental::box2d domain({kXmin, kYmin}, {kXmax, kYmax});
    mln::experimental::image2d<uint32_t> a(domain, bp);
    check_2d(a, 0);
  }
  {
    mln::experimental::image2d<uint32_t> a;
    a.resize(kWidth, kHeight, bp);
    check_2d(a, 0);
  }
  {
    mln::experimental::box2d domain(kWidth, kHeight);
    mln::experimental::image2d<uint32_t> a;
    a.resize(domain, bp);
    check_2d(a, 0);
  }
}


/******************************************/
/****             Test 3D             ****/
/******************************************/


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

TEST(ndbuffer_image, constructor_3d)
{
  {
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, kWidth, kHeight, kDepth);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight, kDepth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain({kXmin, kYmin, kZmin}, {kXmax, kYmax, kZmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight, kDepth);
    check_3d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight, kDepth);
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
    mln::experimental::Box domain(kWidth, kHeight, kDepth);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_3d(a, 0);
  }
  {
    mln::experimental::Box domain({kXmin, kYmin, kZmin}, {kXmax, kYmax, kZmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_3d(a, 0);
  }
  {
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, kWidth, kHeight, kDepth, bp);
    check_3d(a, 0);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight, kDepth);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_3d(a, 0);
  }
}



TEST(image3d, constructors)
{
  {
    mln::experimental::image3d<uint32_t> a(kWidth, kHeight, kDepth);
    check_3d(a, mln::experimental::image3d<uint32_t>::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box3d domain(kWidth, kHeight, kDepth);

    mln::experimental::image3d<uint32_t> a(domain);
    check_3d(a, mln::experimental::image3d<uint32_t>::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box3d domain({kXmin, kYmin, kZmin}, {kXmax, kYmax, kZmax});

    mln::experimental::image3d<uint32_t> a(domain);
    check_3d(a, mln::experimental::image3d<uint32_t>::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::image3d<uint32_t> a;
    a.resize(kWidth, kHeight, kDepth);
    check_3d(a, mln::experimental::image3d<uint32_t>::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::box3d domain(kWidth, kHeight, kDepth);
    mln::experimental::image3d<uint32_t> a;
    a.resize(domain);
    check_3d(a, mln::experimental::image3d<uint32_t>::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::experimental::image3d<uint32_t> a(kWidth, kHeight, kDepth, bp);
    check_3d(a, 0);
  }
  {
    mln::experimental::box3d domain(kWidth, kHeight, kDepth);
    mln::experimental::image3d<uint32_t> a(domain, bp);
    check_3d(a, 0);
  }
  {
    mln::experimental::box3d domain({kXmin, kYmin, kZmin}, {kXmax, kYmax, kZmax});

    mln::experimental::image3d<uint32_t> a(domain, bp);
    check_3d(a, 0);
  }
  {
    mln::experimental::image3d<uint32_t> a;
    a.resize(kWidth, kHeight, kDepth, bp);
    check_3d(a, 0);
  }
  {
    mln::experimental::box3d domain(kWidth, kHeight, kDepth);
    mln::experimental::image3d<uint32_t> a;
    a.resize(domain, bp);
    check_3d(a, 0);
  }
}

/******************************************/
/****             Test 4D             ****/
/******************************************/


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


TEST(ndbuffer_image, constructor_4d)
{
  {
    mln::experimental::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain({kXmin, kYmin, kZmin, kTmin}, {kXmax, kYmax, kZmax, kTmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::experimental::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_4d(a, 0);
  }
  {
    mln::experimental::Box domain({kXmin, kYmin, kZmin, kTmin}, {kXmax, kYmax, kZmax, kTmax});
    mln::ndbuffer_image a(mln::sample_type_id::UINT32, domain, bp);
    check_4d(a, 0);
  }
  {
    mln::experimental::Box domain(kWidth, kHeight, kDepth, kDuration);
    mln::ndbuffer_image a;
    a.resize(mln::sample_type_id::UINT32, domain, bp);
    check_4d(a, 0);
  }
}

TEST(image4d, constructors)
{
  {
    mln::experimental::ndbox<4> domain({kXmin, kYmin, kZmin, kTmin}, {kXmax, kYmax, kZmax, kTmax});

    mln::__ndbuffer_image<uint32_t, 4> a(domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }
  {
    mln::experimental::ndbox<4> domain({kXmin, kYmin, kZmin, kTmin}, {kXmax, kYmax, kZmax, kTmax});

    mln::__ndbuffer_image<uint32_t, 4> a;
    a.resize(domain);
    check_4d(a, mln::ndbuffer_image::DEFAULT_BORDER_SIZE);
  }

  mln::image_build_params bp;
  bp.border = 0;

  {
    mln::experimental::ndbox<4> domain({kXmin, kYmin, kZmin, kTmin}, {kXmax, kYmax, kZmax, kTmax});

    mln::__ndbuffer_image<uint32_t, 4> a(domain, bp);
    check_4d(a, 0);
  }
  {
    mln::experimental::ndbox<4> domain({kXmin, kYmin, kZmin, kTmin}, {kXmax, kYmax, kZmax, kTmax});

    mln::__ndbuffer_image<uint32_t, 4> a;
    a.resize(domain, bp);
    check_4d(a, 0);
  }
}


/********************************************************/
/****             Test Non-trivals types             ****/
/********************************************************/


struct SampleType
{
  enum { DEFAULT_CONSTRUCTED, COPY_CONSTRUCTED, MOVE_CONSTRUCTED };
  static int count[3];
  SampleType() : v{DEFAULT_CONSTRUCTED} { count[v]++; };
  SampleType(const SampleType&) : v{COPY_CONSTRUCTED} { count[v]++; };
  SampleType(SampleType&&) : v{MOVE_CONSTRUCTED} { count[v]++; };
  ~SampleType() { count[v]--; };
  int v;
};

int SampleType::count[3] = {0,0,0};


TEST(ndimage_T, default_constructed_buffer)
{
  {
    mln::image_build_params bp;
    bp.border = 0;

    mln::experimental::image2d<SampleType> a(kWidth, kHeight, bp);
    EXPECT_EQ(kWidth * kHeight, SampleType::count[SampleType::DEFAULT_CONSTRUCTED]);
    EXPECT_EQ(SampleType::DEFAULT_CONSTRUCTED, reinterpret_cast<SampleType*>(a.buffer())[0].v);
  }
  EXPECT_EQ(0, SampleType::count[SampleType::DEFAULT_CONSTRUCTED]);
}

TEST(ndimage_T, copy_constructed_buffer)
{
  {
    mln::image_build_params bp;
    bp.border = 0;
    bp.init_value = SampleType{};

    mln::experimental::image2d<SampleType> a(kWidth, kHeight, bp);
    bp.init_value.reset();

    EXPECT_EQ(kWidth * kHeight, SampleType::count[SampleType::COPY_CONSTRUCTED]);
    EXPECT_EQ(SampleType::COPY_CONSTRUCTED, reinterpret_cast<SampleType*>(a.buffer())[0].v);
  }
  EXPECT_EQ(0, SampleType::count[SampleType::COPY_CONSTRUCTED]);
}



/********************************************************************/
/****             Test Initialisation from Init List             ****/
/********************************************************************/


TEST(ndbuffer_image, init_list_1d)
{
  mln::ndbuffer_image img = {1,2,3};

  EXPECT_EQ(img.pdim(), 1);
  EXPECT_EQ(img.width(), 3);
  EXPECT_EQ(img.domain(), mln::experimental::Box(3));
  EXPECT_EQ(img.sample_type(), mln::sample_type_id::INT32);
  EXPECT_EQ(*static_cast<const int*>(img({0})), 1);
  EXPECT_EQ(*static_cast<const int*>(img({2})), 3);
}



TEST(ndbuffer_image, init_list_2d)
{
  mln::ndbuffer_image img = {{1,2,3}, {4,5,6}};

  EXPECT_EQ(img.pdim(), 2);
  EXPECT_EQ(img.width(), 3);
  EXPECT_EQ(img.height(), 2);
  EXPECT_EQ(img.domain(), mln::experimental::Box(3, 2));
  EXPECT_EQ(img.sample_type(), mln::sample_type_id::INT32);
  EXPECT_EQ(*static_cast<const int*>(img({0,0})), 1);
  EXPECT_EQ(*static_cast<const int*>(img({2,0})), 3);
  EXPECT_EQ(*static_cast<const int*>(img({0,1})), 4);
  EXPECT_EQ(*static_cast<const int*>(img({2,1})), 6);
}

TEST(ndbuffer_image, init_list_3d)
{
  mln::ndbuffer_image img = {
    {{1,2,3},
     {4,5,6}},
    {{7,8,9},
     {11,12,13}},
  };

  EXPECT_EQ(img.pdim(), 3);
  EXPECT_EQ(img.width(), 3);
  EXPECT_EQ(img.height(), 2);
  EXPECT_EQ(img.depth(), 2);
  EXPECT_EQ(img.domain(), mln::experimental::Box(3, 2, 2));
  EXPECT_EQ(img.sample_type(), mln::sample_type_id::INT32);
  EXPECT_EQ(*static_cast<const int*>(img({0,0,0})), 1);
  EXPECT_EQ(*static_cast<const int*>(img({2,0,0})), 3);
  EXPECT_EQ(*static_cast<const int*>(img({0,1,0})), 4);
  EXPECT_EQ(*static_cast<const int*>(img({2,1,0})), 6);
  EXPECT_EQ(*static_cast<const int*>(img({0,0,1})), 7);
  EXPECT_EQ(*static_cast<const int*>(img({2,0,1})), 9);
  EXPECT_EQ(*static_cast<const int*>(img({0,1,1})), 11);
  EXPECT_EQ(*static_cast<const int*>(img({2,1,1})), 13);
}


TEST(image1d, init_list)
{
  mln::experimental::image1d<int> img = {1,2,3};

  EXPECT_EQ(img.pdim(), 1);
  EXPECT_EQ(img.width(), 3);
  EXPECT_EQ(img.domain(), mln::experimental::Box(3));
  EXPECT_EQ(img.sample_type(), mln::sample_type_id::INT32);
  EXPECT_EQ(img({0}), 1);
  EXPECT_EQ(img({2}), 3);
}



TEST(image2d, init_list)
{
  mln::experimental::image2d<int> img = {{1,2,3}, {4,5,6}};

  EXPECT_EQ(img.pdim(), 2);
  EXPECT_EQ(img.width(), 3);
  EXPECT_EQ(img.height(), 2);
  EXPECT_EQ(img.domain(), mln::experimental::Box(3, 2));
  EXPECT_EQ(img.sample_type(), mln::sample_type_id::INT32);
  EXPECT_EQ(img({0,0}), 1);
  EXPECT_EQ(img({2,0}), 3);
  EXPECT_EQ(img({0,1}), 4);
  EXPECT_EQ(img({2,1}), 6);
}

TEST(image3d, init_list)
{
  mln::experimental::image3d<int> img = {
    {{1,2,3},
     {4,5,6}},
    {{7,8,9},
     {11,12,13}},
  };

  EXPECT_EQ(img.pdim(), 3);
  EXPECT_EQ(img.width(), 3);
  EXPECT_EQ(img.height(), 2);
  EXPECT_EQ(img.depth(), 2);
  EXPECT_EQ(img.domain(), mln::experimental::Box(3, 2, 2));
  EXPECT_EQ(img.sample_type(), mln::sample_type_id::INT32);
  EXPECT_EQ(img({0,0,0}), 1);
  EXPECT_EQ(img({2,0,0}), 3);
  EXPECT_EQ(img({0,1,0}), 4);
  EXPECT_EQ(img({2,1,0}), 6);
  EXPECT_EQ(img({0,0,1}), 7);
  EXPECT_EQ(img({2,0,1}), 9);
  EXPECT_EQ(img({0,1,1}), 11);
  EXPECT_EQ(img({2,1,1}), 13);
}



/********************************************************************/
/****             Test Clipping                                  ****/
/********************************************************************/


TEST(ndbuffer_image, clip_valid_roi)
{
  mln::image_build_params params;
  params.border = 0;

  mln::ndbuffer_image img(kSampleType, kWidth, kHeight, params);
  iota(img);

  mln::experimental::Box roi({2,5}, {7,10});
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0,0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({6,9})), 96);


  auto f = img.clip(roi);
  EXPECT_EQ(f.domain(), roi);
  EXPECT_EQ(*static_cast<const uint32_t*>(f({2,5})), 52);
  EXPECT_EQ(*static_cast<const uint32_t*>(f({6,9})), 96);
  EXPECT_EQ(f.width(), 5);
  EXPECT_EQ(f.height(), 5);
  EXPECT_EQ(f.domain(), roi);
}

TEST(ndbuffer_image, clip_invalid_roi)
{
  mln::image_build_params params;
  params.border = 0;

  mln::ndbuffer_image img(kSampleType, kWidth, kHeight, params);
  iota(img);

  mln::experimental::Box roi({2, 5}, {kWidth + 1, kHeight + 1});

  EXPECT_THROW(img.clip(roi), std::runtime_error);
}

/********************************************************************/
/****             Test Border Size Computation                   ****/
/********************************************************************/

TEST(ndbuffer_image, compute_border)
{
  mln::image_build_params params;
  params.border = 0;

  mln::ndbuffer_image img(kSampleType, kWidth, kHeight, params);


  mln::experimental::Box roi({2,5}, {7,10});


  auto f = img.clip(roi);
  int  left_border   = roi.tl().x();
  int  right_border  = kWidth - roi.br().x();
  int  up_border     = roi.tl().y();
  int  bottom_border = kHeight - roi.br().y();


  EXPECT_EQ(f.border(), std::min({left_border, right_border, up_border, bottom_border}));
}



/********************************************************************/
/****             Test from_buffer                               ****/
/********************************************************************/


TEST(ndbuffer_image, from_buffer_1d_view)
{
  std::vector<uint32_t> v(kWidth, 42);

  int sizes[] = {kWidth};
  auto img = mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(v.data()), mln::sample_type_id::UINT32, 1, sizes);
  iota(img);

  EXPECT_EQ(img.pdim(), 1);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth - 1})), kWidth - 1);

  std::vector<uint32_t> v2(kWidth);
  std::iota(v2.begin(), v2.end(), 0);
  EXPECT_EQ(v, v2);
}

TEST(image1d, from_buffer_view)
{
  std::vector<uint32_t> v(kWidth, 42);

  int sizes[] = {kWidth};
  auto img = mln::experimental::image1d<uint32_t>::from_buffer(v.data(), sizes);
  iota(img);

  EXPECT_EQ(img.pdim(), 1);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img({0}), 0);
  EXPECT_EQ(img({kWidth - 1}), kWidth - 1);

  std::vector<uint32_t> v2(kWidth);
  std::iota(v2.begin(), v2.end(), 0);
  EXPECT_EQ(v, v2);
}


TEST(ndbuffer_image, from_buffer_1d_copy)
{
  std::vector<uint32_t> v(kWidth, 42);

  bool copy = true;
  int sizes[] = {kWidth};
  auto img = mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(v.data()), mln::sample_type_id::UINT32, 1, sizes, nullptr, copy);
  iota(img);

  EXPECT_EQ(img.pdim(), 1);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth - 1})), kWidth - 1);

  EXPECT_EQ(v, std::vector<uint32_t>(kWidth, 42));
}

TEST(image1d, from_buffer_copy)
{
  std::vector<uint32_t> v(kWidth, 42);

  bool copy = true;
  int sizes[] = {kWidth};
  auto img = mln::experimental::image1d<uint32_t>::from_buffer(v.data(), sizes, nullptr, copy);
  iota(img);

  EXPECT_EQ(img.pdim(), 1);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img({0}), 0);
  EXPECT_EQ(img({kWidth - 1}), kWidth - 1);

  EXPECT_EQ(v, std::vector<uint32_t>(kWidth, 42));
}



TEST(ndbuffer_image, from_buffer_2d_view)
{
  std::vector<uint32_t> v(kWidth * kHeight, 42);

  int sizes[] = {kWidth, kHeight};
  auto img = mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(v.data()), mln::sample_type_id::UINT32, 2, sizes);
  iota(img);

  EXPECT_EQ(img.pdim(), 2);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0, 0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth - 1, kHeight - 1})), (kWidth * kHeight) - 1);

  std::vector<uint32_t> v2(kWidth * kHeight);
  std::iota(v2.begin(), v2.end(), 0);
  EXPECT_EQ(v, v2);
}

TEST(image2d, from_buffer_view)
{
  std::vector<uint32_t> v(kWidth * kHeight, 42);

  int sizes[] = {kWidth, kHeight};
  auto img = mln::experimental::image2d<uint32_t>::from_buffer(v.data(), sizes);
  iota(img);

  EXPECT_EQ(img.pdim(), 2);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(img({0, 0}), 0);
  EXPECT_EQ(img({kWidth - 1, kHeight - 1}), (kWidth * kHeight) - 1);

  std::vector<uint32_t> v2(kWidth * kHeight);
  std::iota(v2.begin(), v2.end(), 0);
  EXPECT_EQ(v, v2);
}

TEST(ndbuffer_image, from_buffer_2d_copy)
{
  std::vector<uint32_t> v(kWidth * kHeight, 42);

  bool copy = true;
  int sizes[] = {kWidth, kHeight};
  auto img = mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(v.data()), mln::sample_type_id::UINT32, 2, sizes, nullptr, copy);
  iota(img);

  EXPECT_EQ(img.pdim(), 2);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0, 0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth - 1, kHeight - 1})), (kWidth * kHeight) - 1);

  EXPECT_EQ(v, std::vector<uint32_t>(kWidth * kHeight, 42));
}

TEST(image2d, from_buffer_copy)
{
  std::vector<uint32_t> v(kWidth * kHeight, 42);

  bool copy = true;
  int sizes[] = {kWidth, kHeight};
  auto img = mln::experimental::image2d<uint32_t>::from_buffer(v.data(), sizes, nullptr, copy);
  iota(img);

  EXPECT_EQ(img.pdim(), 2);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(img({0, 0}), 0);
  EXPECT_EQ(img({kWidth - 1, kHeight - 1}), (kWidth * kHeight) - 1);

  EXPECT_EQ(v, std::vector<uint32_t>(kWidth * kHeight, 42));
}



TEST(ndbuffer_image, from_buffer_3d_view)
{
  std::vector<uint32_t> v(kWidth * kHeight * kDepth, 42);

  int sizes[] = {kWidth, kHeight, kDepth};
  auto img = mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(v.data()), mln::sample_type_id::UINT32, 3, sizes);
  iota(img);

  EXPECT_EQ(img.pdim(), 3);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(img.depth(), kDepth);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0, 0, 0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth - 1, kHeight - 1, kDepth - 1})), (kWidth * kHeight * kDepth) - 1);

  std::vector<uint32_t> v2(kWidth * kHeight * kDepth);
  std::iota(v2.begin(), v2.end(), 0);
  EXPECT_EQ(v, v2);
}


TEST(image3d, from_buffer_view)
{
  std::vector<uint32_t> v(kWidth * kHeight * kDepth, 42);

  int sizes[] = {kWidth, kHeight, kDepth};
  auto img = mln::experimental::image3d<uint32_t>::from_buffer(v.data(), sizes);
  iota(img);

  EXPECT_EQ(img.pdim(), 3);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(img.depth(), kDepth);
  EXPECT_EQ(img({0, 0, 0}), 0);
  EXPECT_EQ(img({kWidth - 1, kHeight - 1, kDepth - 1}), (kWidth * kHeight * kDepth) - 1);

  std::vector<uint32_t> v2(kWidth * kHeight * kDepth);
  std::iota(v2.begin(), v2.end(), 0);
  EXPECT_EQ(v, v2);
}



TEST(ndbuffer_image, from_buffer_3d_copy)
{
  std::vector<uint32_t> v(kWidth * kHeight * kDepth, 42);

  bool copy = true;
  int sizes[] = {kWidth, kHeight, kDepth};
  auto img = mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(v.data()), mln::sample_type_id::UINT32, 3, sizes, nullptr, copy);
  iota(img);

  EXPECT_EQ(img.pdim(), 3);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(img.depth(), kDepth);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0, 0, 0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth - 1, kHeight - 1, kDepth - 1})), (kWidth * kHeight * kDepth) - 1);

  EXPECT_EQ(v, std::vector<uint32_t>(kWidth * kHeight * kDepth, 42));
}


TEST(image3d, from_buffer_copy)
{
  std::vector<uint32_t> v(kWidth * kHeight * kDepth, 42);

  bool copy = true;
  int sizes[] = {kWidth, kHeight, kDepth};
  auto img = mln::experimental::image3d<uint32_t>::from_buffer(v.data(), sizes, nullptr, copy);
  iota(img);

  EXPECT_EQ(img.pdim(), 3);
  EXPECT_EQ(img.width(), kWidth);
  EXPECT_EQ(img.height(), kHeight);
  EXPECT_EQ(img.depth(), kDepth);
  EXPECT_EQ(img({0, 0, 0}), 0);
  EXPECT_EQ(img({kWidth - 1, kHeight - 1, kDepth - 1}), (kWidth * kHeight * kDepth) - 1);

  EXPECT_EQ(v, std::vector<uint32_t>(kWidth * kHeight * kDepth, 42));
}



/********************************************************************/
/****             Test domain inflate                            ****/
/********************************************************************/

TEST(ndbuffer_image, inflate_domain)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  // FIXME: init value is ignored with ndbuffer_image generic constructor
  mln::ndbuffer_image img = mln::experimental::image2d<uint32_t>(kWidth, kHeight, params);
  iota(img);

  img.inflate_domain(2);
  EXPECT_EQ(img.domain(), mln::experimental::Box({-2, -2}, {kWidth + 2, kHeight + 2}));
  EXPECT_EQ(*static_cast<const uint32_t*>(img({-2, -2})), 42);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({0, 0})), 0);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth - 1, kHeight - 1})), (kWidth * kHeight) - 1);
  EXPECT_EQ(*static_cast<const uint32_t*>(img({kWidth + 1, kHeight + 1})), 42);
  EXPECT_EQ(img.border(), 1);
}

TEST(image2d, inflate_domain)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  // FIXME: init value is ignored with ndbuffer_image generic constructor
  mln::experimental::image2d<uint32_t> img(kWidth, kHeight, params);
  iota(img);

  img.inflate_domain(2);
  EXPECT_EQ(img.domain(), mln::experimental::box2d({-2, -2}, {kWidth + 2, kHeight + 2}));
  EXPECT_EQ(img({-2, -2}), 42);
  EXPECT_EQ(img({0, 0}), 0);
  EXPECT_EQ(img({kWidth - 1, kHeight - 1}), (kWidth * kHeight) - 1);
  EXPECT_EQ(img({kWidth + 1, kHeight + 1}), 42);
  EXPECT_EQ(img.border(), 1);
}

TEST(ndbuffer_image, deflated_domain)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  mln::ndbuffer_image img = mln::experimental::image2d<uint32_t>(kWidth, kHeight, params);
  iota(img);

  img.inflate_domain(-2);
  ASSERT_EQ(img.domain(), mln::experimental::Box({2, 2}, {kWidth - 2, kHeight - 2}));
  ASSERT_EQ(img.border(), 5);
}

TEST(image2d, deflated_domain)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  mln::experimental::image2d<uint32_t> img(kWidth, kHeight, params);
  iota(img);

  img.inflate_domain(-2);
  ASSERT_EQ(img.domain(), mln::experimental::box2d({2, 2}, {kWidth - 2, kHeight - 2}));
  ASSERT_EQ(img.border(), 5);
}



/********************************************************************/
/****             Test accessors and index-related methods       ****/
/********************************************************************/

TEST(ndbuffer_image, indexes_negative_start)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  mln::experimental::box2d             domain = {{-2, -3}, {4, 5}};
  mln::ndbuffer_image img = mln::experimental::image2d<uint32_t>(domain, params);
  std::ptrdiff_t s = img.stride();
  iota(img);

  for (int y = -6; y < 8; ++y)
    for (int x = -5; x < 7; ++x)
    {
      std::ptrdiff_t idx = y * s + x;
      EXPECT_EQ(img.index_of_point({x,y}), idx);
      EXPECT_EQ(img.point_at_index(idx), (mln::experimental::Point{x, y}));
      EXPECT_EQ(img[idx], img.at({x,y}));
    }
}

TEST(image2d, indexes_negative_start)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  mln::experimental::box2d             domain = {{-2, -3}, {4, 5}};
  mln::experimental::image2d<uint32_t> img(domain, params);
  std::ptrdiff_t                       s = img.stride();
  iota(img);

  for (int y = -6; y < 8; ++y)
    for (int x = -5; x < 7; ++x)
    {
      std::ptrdiff_t idx = y * s + x;
      EXPECT_EQ(img.index_of_point({x,y}), idx);
      EXPECT_EQ(img.point_at_index(idx), (mln::experimental::point2d{x, y}));
      EXPECT_EQ(img[idx], img.at({x,y}));
      auto px = img.new_pixel_at({x,y});
      EXPECT_EQ(px.val(), img.at({x,y}));
      EXPECT_EQ(px.point(), (mln::experimental::point2d{x, y}));
    }
}


TEST(ndbuffer_image, indexes_positive_start)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  mln::experimental::box2d domain = {{4, 5}, {10, 11}};
  mln::ndbuffer_image      img    = mln::experimental::image2d<uint32_t>(domain, params);
  std::ptrdiff_t           s      = img.stride();
  iota(img);

  for (int y = 2; y < 14; ++y)
    for (int x = 1; x < 13; ++x)
    {
      std::ptrdiff_t idx = y * s + x;
      EXPECT_EQ(img.index_of_point({x,y}), idx);
      EXPECT_EQ(img.point_at_index(idx), (mln::experimental::Point{x, y}));
      EXPECT_EQ(img[idx], img.at({x,y}));
    }
}

TEST(image2d, indexes_positive_start)
{
  mln::image_build_params params;
  params.border = 3;
  params.init_value = uint32_t(42);

  mln::experimental::box2d             domain = {{4, 5}, {10, 11}};
  mln::experimental::image2d<uint32_t> img(domain, params);
  std::ptrdiff_t                       s = img.stride();
  iota(img);

  for (int y = 2; y < 14; ++y)
    for (int x = 1; x < 13; ++x)
    {
      std::ptrdiff_t idx = y * s + x;
      EXPECT_EQ(img.index_of_point({x,y}), idx);
      EXPECT_EQ(img.point_at_index(idx), (mln::experimental::point2d{x, y}));
      EXPECT_EQ(img[idx], img.at({x,y}));
      auto px = img.new_pixel_at({x,y});
      EXPECT_EQ(px.val(), img.at({x,y}));
      EXPECT_EQ(px.point(), (mln::experimental::point2d{x, y}));
    }
}

/********************************************************************/
/****             Test value ranges                              ****/
/********************************************************************/

template <class I>
void image_test_read(I& img)
{
  auto vals = img.new_values();
  int i = 0;
  for (auto row : mln::ranges::rows(vals))
    for (auto v : row)
      ASSERT_EQ(i++, v);
}

template <class I>
void image_test_write(I& img)
{
  mln::image_concrete_t<I> ref = mln::imconcretize(img);
  iota(ref);

  auto vals = img.new_values();
  int i = 0;
  for (auto row : mln::ranges::rows(vals))
    for (auto& v : row)
      v = i++;
  ASSERT_IMAGES_EQ_EXP(img, ref);
}

TEST(image2d, value_range_read)
{
  // Empty domain
  {
    mln::experimental::image2d<uint32_t> img;
    iota(img);
    image_test_read(img);
  }
  // Non-empty domain
  {
    mln::experimental::image2d<uint32_t> img(3, 4);
    iota(img);
    image_test_read(img);
  }
  // As const image
  {
    mln::experimental::image2d<uint32_t> img(3, 4);
    iota(img);
    image_test_read(std::as_const(img));
  }
}


TEST(image2d, value_range_write)
{
  // Empty domain
  {
    mln::experimental::image2d<uint32_t> img;
    image_test_write(img);
  }
  // Non-empty domain
  {
    mln::experimental::image2d<uint32_t> img(3, 4);
    image_test_write(img);
  }
}


/********************************************************************/
/****             Test pixel ranges                              ****/
/********************************************************************/


template <class I>
void image_test_pix_read(I& img)
{
  auto dom = img.domain();
  auto vals = img.new_pixels();
  int i = 0;
  int x = dom.tl(0);
  int y = dom.tl(1);
  for (auto row : mln::ranges::rows(vals))
  {
    x = dom.tl(0);
    for (auto px : row)
    {
      ASSERT_EQ(px.val(), i++);
      ASSERT_EQ(px.point(), (mln::experimental::ndpoint<2, std::ptrdiff_t>{x,y}));
      x++;
    }
    y++;
  }
}

template <class I>
void image_test_pix_write(I& img)
{
  mln::image_concrete_t<I> ref = mln::imconcretize(img);
  iota(ref);

  auto dom = img.domain();
  auto vals = img.new_pixels();
  int i = 0;
  int x = dom.tl(0);
  int y = dom.tl(1);
  for (auto row : mln::ranges::rows(vals))
  {
    x = dom.tl(0);
    for (auto px : row)
    {
      px.val() = i++;
      ASSERT_EQ(px.point(), (mln::experimental::ndpoint<2, std::ptrdiff_t>{x,y}));
      x++;
    }
    y++;
  }

  ASSERT_IMAGES_EQ_EXP(img, ref);
}

TEST(image2d, pixel_range_read)
{
  // Empty domain
  {
    mln::experimental::image2d<uint32_t> img;
    iota(img);
    image_test_pix_read(img);
  }
  // Non-empty domain
  {
    mln::experimental::image2d<uint32_t> img(3, 4);
    iota(img);
    image_test_pix_read(img);
  }
  // As const image
  {
    mln::experimental::image2d<uint32_t> img(3, 4);
    iota(img);
    image_test_pix_read(std::as_const(img));
  }
}


TEST(image2d, pixel_range_write)
{
  // Empty domain
  {
    mln::experimental::image2d<uint32_t> img;
    image_test_pix_write(img);
  }
  // Non-empty domain
  {
    mln::experimental::image2d<uint32_t> img(3, 4);
    image_test_pix_write(img);
  }

}



