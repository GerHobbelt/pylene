#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/morpho/watershed.hpp>

#include <gtest/gtest.h>
#include <tests/helpers.hpp>


using namespace mln;

TEST(Morpho, watershed_gray)
{
  const mln::image2d<mln::uint8> input = {
    { 2,  2,  2,  2,  2},
    {40, 30, 30, 30, 40},
    {40, 20, 20, 20, 40},
    {40, 40, 20, 40, 40},
    { 1,  5, 20,  5,  1}};

  const mln::image2d<mln::int16> ref = {
    { 1,  1,  1,  1,  1},
    { 1,  1,  1,  1,  1},
    { 0,  1,  1,  1,  0},
    { 2,  0,  1,  0,  3},
    { 2,  2,  0,  3,  3}};

  int nlabel;
  auto res = mln::morpho::watershed<mln::int8>(input, mln::c4, nlabel);
  ASSERT_IMAGES_EQ(ref, res);
}

TEST(Morpho, watershed_thick)
{
  const mln::image2d<mln::uint8> input = {
    { 0, 10,  0, 10,  0},
    { 0, 10, 10, 10, 10},
    {10, 10, 10, 10, 10},
    { 0, 10, 10, 10, 10},
    { 0, 10,  0, 10,  0}};

  const mln::image2d<mln::int16> ref = {
    { 1,  0,  2,  0,  3},
    { 1,  1,  0,  3,  3},
    { 0,  0,  0,  0,  0},
    { 4,  4,  0,  6,  6},
    { 4,  0,  5,  0,  6}};

  int nlabel;
  auto res = mln::morpho::watershed<mln::int16>(input, mln::c4, nlabel);
  ASSERT_IMAGES_EQ(ref, res);
}


