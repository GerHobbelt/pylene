#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>


#include <gtest/gtest.h>
#include <tests/helpers.hpp>
#include <mln/labeling/local_extrema.hpp>
using namespace mln;


TEST(Morpho, local_minima)
{
  const mln::image2d<mln::uint8> input = {
    { 2,  2,  2,  2,  2},
    {40, 30, 30, 30, 40},
    {40, 20, 20, 20, 40},
    {40, 40, 20, 40, 40},
    { 1,  5, 20,  5,  1}};

  const mln::image2d<mln::int8> ref = {
    { 1,  1,  1,  1,  1},
    { 0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0},
    { 2,  0,  0,  0,  3}};

  int nlabel;
  auto res = mln::labeling::local_minima<mln::int8>(input, mln::c4, nlabel);
  ASSERT_EQ(3, nlabel);
  ASSERT_IMAGES_EQ(ref, res);
}

TEST(Morpho, local_maxima)
{
  const mln::image2d<mln::uint8> input = {
    { 2,  2,  2,  2,  2},
    {40, 30, 30, 30, 40},
    {40, 20, 20, 20, 40},
    {40, 40, 20, 40, 40},
    { 1,  5, 20,  5,  1}};

  const mln::image2d<mln::int8> ref = {
    { 0,  0,  0,  0,  0},
    { 1,  0,  0,  0,  2},
    { 1,  0,  0,  0,  2},
    { 1,  1,  0,  2,  2},
    { 0,  0,  0,  0,  0}};

  int nlabel;
  auto res = mln::labeling::local_maxima<mln::int8>(input, mln::c4, nlabel);
  ASSERT_EQ(2, nlabel);
  ASSERT_IMAGES_EQ(ref, res);
}
