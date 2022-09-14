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
}


void test_extract_observations(std::vector<Eigen::Matrix<float, 3, 1>>& ref, const mln::image2d<uint8_t>& image,
                               const scribo::SegDetParams& params)
{
  auto descriptor = scribo::internal::Descriptor(params, 0);
  auto output     = scribo::internal::extract_observations(image, 0, image.height(), descriptor);

  EXPECT_EQ(output.size(), ref.size());
  for (size_t i = 0; i < output.size(); i++)
  {
    EXPECT_EQ(output[i](0, 0), ref[i](0, 0));
    EXPECT_EQ(output[i](1, 0), ref[i](1, 0));
  }
}

TEST(Segdet, no_observation_binary)
{
  using namespace scribo;
  using namespace scribo::internal;

  mln::image2d<std::uint8_t> image = mln::image2d<std::uint8_t>(5, 1);
  image({0, 0})                    = WHITE;
  image({1, 0})                    = WHITE;
  image({2, 0})                    = WHITE;
  image({3, 0})                    = WHITE;
  image({4, 0})                    = WHITE;

  std::vector<Eigen::Matrix<float, 3, 1>> ref;

  auto params            = SegDetParams();
  params.extraction_type = SEGDET_PROCESS_EXTRACTION_ENUM::BINARY;
  params.max_max_llum    = GREY_WHITE;
  params.max_llum        = GREY_WHITE;

  test_extract_observations(ref, image, params);
}
