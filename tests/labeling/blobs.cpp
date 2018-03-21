#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imprint.hpp>
#include <mln/labeling/blobs.hpp>

#include <gtest/gtest.h>

TEST(Labeling, blobs_fast)
{
  using namespace mln;

  image2d<uint8> ima(5, 5);
  iota(ima, 0);

  image2d<bool> mask = eval(ima % 2 == 0);

  image2d<uint8> lbl;
  unsigned nlabel;
  std::tie(lbl, nlabel) = labeling::blobs(mask, c4, uint8());
  ASSERT_EQ(nlabel, 13u);
}

TEST(Labeling, blobs_custom)
{
  using namespace mln;

  image2d<uint8> ima(5, 5);
  iota(ima, 0);

  image2d<uint8> lbl;
  unsigned nlabel;

  std::tie(lbl, nlabel) = labeling::blobs(ima % 2 == 0, c4, uint8());
  ASSERT_EQ(nlabel, 13u);

  std::tie(lbl, nlabel) = labeling::blobs(ima % 2 == 0, c8, uint8());
  ASSERT_EQ(nlabel, 1u);
}
