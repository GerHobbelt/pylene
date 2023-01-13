#include <gtest/gtest.h>

#include <mln/core/image/ndimage.hpp>

#include "../../pylene/src/scribo/segdet/process/extractors/extract_observation.hpp"

namespace
{
#define WHITE 255
#define GREY_WHITE 172
#define GREY 128
#define BLACK_GREY 64
#define BLACK 0
} // namespace


void test_extract_observations(std::vector<Eigen::Matrix<float, 3, 1>>& ref, const mln::image2d<uint8_t>& image,
                               const scribo::SegDetParams& params)
{
  auto descriptor = scribo::internal::Descriptor(params, 0);
  auto output     = scribo::internal::extract_observations(image, 0, image.height(), descriptor);

  ASSERT_EQ(output.size(), ref.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    EXPECT_EQ(output[i](0, 0), ref[i](0, 0));
    EXPECT_EQ(output[i](1, 0), ref[i](1, 0));
    EXPECT_EQ(output[i](2, 0), ref[i](2, 0));
  }
}

TEST(Segdet, binary_none)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = WHITE;
  image({0, 1})                    = WHITE;
  image({0, 2})                    = WHITE;
  image({0, 3})                    = WHITE;
  image({0, 4})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;

  auto params            = SegDetParams();
  params.extraction_type = SEGDET_PROCESS_EXTRACTION_ENUM::BINARY;
  params.blumi           = GREY_WHITE;
  params.llumi        = GREY_WHITE;

  test_extract_observations(ref, image, params);
}

TEST(Segdet, binary_odd)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = WHITE;
  image({0, 1})                    = BLACK;
  image({0, 2})                    = BLACK;
  image({0, 3})                    = BLACK;
  image({0, 4})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;
  ref.push_back(Eigen::Matrix<float, 3, 1>(2, 3, BLACK));

  auto params            = SegDetParams();
  params.extraction_type = SEGDET_PROCESS_EXTRACTION_ENUM::BINARY;
  params.blumi           = GREY_WHITE;
  params.llumi        = GREY_WHITE;

  test_extract_observations(ref, image, params);
}

TEST(Segdet, binary_even_end)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = WHITE;
  image({0, 1})                    = BLACK;
  image({0, 2})                    = BLACK;
  image({0, 3})                    = BLACK;
  image({0, 4})                    = BLACK;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;
  ref.push_back(Eigen::Matrix<float, 3, 1>(2.5f, 4, BLACK));

  auto params            = SegDetParams();
  params.extraction_type = SEGDET_PROCESS_EXTRACTION_ENUM::BINARY;
  params.blumi           = GREY_WHITE;
  params.llumi        = GREY_WHITE;

  test_extract_observations(ref, image, params);
}

TEST(Segdet, binary_multiple)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = BLACK;
  image({0, 1})                    = BLACK;
  image({0, 2})                    = WHITE;
  image({0, 3})                    = GREY;
  image({0, 4})                    = GREY;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;
  ref.push_back(Eigen::Matrix<float, 3, 1>(0.5f, 2, BLACK));
  ref.push_back(Eigen::Matrix<float, 3, 1>(3.5f, 2, GREY));

  auto params            = SegDetParams();
  params.extraction_type = SEGDET_PROCESS_EXTRACTION_ENUM::BINARY;
  params.blumi           = GREY_WHITE;
  params.llumi        = GREY_WHITE;

  test_extract_observations(ref, image, params);
}

TEST(Segdet, binary_even_begin)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = BLACK;
  image({0, 1})                    = BLACK;
  image({0, 2})                    = BLACK;
  image({0, 3})                    = BLACK;
  image({0, 4})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;
  ref.push_back(Eigen::Matrix<float, 3, 1>(1.5f, 4, BLACK));

  auto params            = SegDetParams();
  params.extraction_type = SEGDET_PROCESS_EXTRACTION_ENUM::BINARY;
  params.blumi           = GREY_WHITE;
  params.llumi        = GREY_WHITE;

  test_extract_observations(ref, image, params);
}

TEST(Segdet, gradient_none)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = WHITE;
  image({0, 1})                    = WHITE;
  image({0, 2})                    = WHITE;
  image({0, 3})                    = WHITE;
  image({0, 4})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;

  auto params               = SegDetParams();
  params.extraction_type    = SEGDET_PROCESS_EXTRACTION_ENUM::GRADIENT;
  params.gradient_threshold = 32;

  test_extract_observations(ref, image, params);
}

TEST(Segdet, gradient_two)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = WHITE;
  image({0, 1})                    = BLACK; // 1
  image({0, 2})                    = BLACK;
  image({0, 3})                    = BLACK; // 2
  image({0, 4})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;
  ref.push_back(Eigen::Matrix<float, 3, 1>(1, 1, BLACK - WHITE)); // 1
  ref.push_back(Eigen::Matrix<float, 3, 1>(3, 1, WHITE - BLACK)); // 2

  auto params               = SegDetParams();
  params.extraction_type    = SEGDET_PROCESS_EXTRACTION_ENUM::GRADIENT;
  params.gradient_threshold = 32;

  test_extract_observations(ref, image, params);
}


TEST(Segdet, gradient_double)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 7);
  image({0, 0})                    = WHITE;
  image({0, 1})                    = WHITE; // 1
  image({0, 2})                    = GREY;  // 1
  image({0, 3})                    = BLACK;
  image({0, 4})                    = GREY;  // 2
  image({0, 5})                    = WHITE; // 2
  image({0, 6})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;
  ref.push_back(Eigen::Matrix<float, 3, 1>(1.5f, 2, BLACK - WHITE)); // 1
  ref.push_back(Eigen::Matrix<float, 3, 1>(4.5f, 2, WHITE - BLACK)); // 2

  auto params               = SegDetParams();
  params.extraction_type    = SEGDET_PROCESS_EXTRACTION_ENUM::GRADIENT;
  params.gradient_threshold = 32;

  test_extract_observations(ref, image, params);
}

TEST(Segdet, gradient_smallest)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(1, 5);
  image({0, 0})                    = WHITE;
  image({0, 1})                    = WHITE; // 1
  image({0, 2})                    = BLACK;
  image({0, 3})                    = WHITE; // 2
  image({0, 4})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;
  ref.push_back(Eigen::Matrix<float, 3, 1>(1, 1, BLACK - WHITE)); // 1
  ref.push_back(Eigen::Matrix<float, 3, 1>(3, 1, WHITE - BLACK)); // 2

  auto params               = SegDetParams();
  params.extraction_type    = SEGDET_PROCESS_EXTRACTION_ENUM::GRADIENT;
  params.gradient_threshold = 32;

  test_extract_observations(ref, image, params);
}
