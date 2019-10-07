#include <mln/morpho/experimental/reconstruction.hpp>

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>


#include <gtest/gtest.h>


TEST(Morpho, opening_by_reconstruction_binary)
{
  mln::experimental::image2d<bool> ima = {{0, 1, 0, 0, 1}, //
                                          {0, 1, 0, 0, 1}, //
                                          {0, 1, 1, 1, 1}, //
                                          {1, 0, 0, 0, 0}, //
                                          {0, 1, 1, 0, 0}, //
                                          {0, 0, 0, 0, 1}};

  mln::experimental::image2d<bool> markers = {{0, 0, 0, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}};

  mln::experimental::image2d<bool> expected = {{0, 1, 0, 0, 1}, //
                                               {0, 1, 0, 0, 1}, //
                                               {0, 1, 1, 1, 1}, //
                                               {0, 0, 0, 0, 0}, //
                                               {0, 1, 1, 0, 0}, //
                                               {0, 0, 0, 0, 0}};
  auto out = mln::morpho::experimental::opening_by_reconstruction(ima, markers, mln::experimental::c4);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}

TEST(Morpho, opening_by_reconstruction_binary_c8)
{
  mln::experimental::image2d<bool> ima = {{0, 1, 0, 0, 1}, //
                                          {0, 1, 0, 0, 1}, //
                                          {0, 1, 1, 1, 1}, //
                                          {1, 0, 0, 0, 0}, //
                                          {0, 1, 1, 0, 0}, //
                                          {0, 0, 0, 0, 1}};

  mln::experimental::image2d<bool> markers = {{0, 0, 0, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}};

  mln::experimental::image2d<bool> expected = {{0, 1, 0, 0, 1}, //
                                               {0, 1, 0, 0, 1}, //
                                               {0, 1, 1, 1, 1}, //
                                               {1, 0, 0, 0, 0}, //
                                               {0, 1, 1, 0, 0}, //
                                               {0, 0, 0, 0, 0}};
  auto out = mln::morpho::experimental::opening_by_reconstruction(ima, markers, mln::experimental::c8);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}



TEST(Morpho, opening_by_reconstruction_gray)
{
  mln::experimental::image2d<uint8_t> ima = {{0, 10, +0, +0, 11}, //
                                             {0, 11, +0, +0, 12}, //
                                             {0, 12, 13, 14, 13}, //
                                             {1, +0, +0, +0, +0}, //
                                             {0, +1, +1, +0, +0}, //
                                             {0, +0, +0, +0, +1}};

  mln::experimental::image2d<uint8_t> markers = {{0, 0, 0, 0, 0},  //
                                                 {0, 0, 0, 0, 0},  //
                                                 {0, 12, 0, 0, 0}, //
                                                 {0, 0, 0, 0, 0},  //
                                                 {0, 0, 1, 0, 0},  //
                                                 {0, 0, 0, 0, 0}};

  mln::experimental::image2d<uint8_t> expected = {{0, 10, 0, 0, 11},   //
                                                  {0, 11, 0, 0, 12},   //
                                                  {0, 12, 12, 12, 12}, //
                                                  {0, 0, 0, 0, 0},     //
                                                  {0, 1, 1, 0, 0},     //
                                                  {0, 0, 0, 0, 0}};

  auto out = mln::morpho::experimental::opening_by_reconstruction(ima, markers, mln::experimental::c4);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}

TEST(Morpho, closing_by_reconstruction_binary)
{
  mln::experimental::image2d<bool> ima = {{0, 1, 0, 0, 1}, //
                                          {0, 1, 0, 0, 1}, //
                                          {0, 1, 1, 1, 1}, //
                                          {1, 0, 0, 0, 0}, //
                                          {0, 1, 1, 0, 0}, //
                                          {0, 0, 0, 0, 1}};

  mln::experimental::image2d<bool> markers = {{1, 1, 1, 0, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {0, 1, 1, 1, 1}};

  mln::experimental::image2d<bool> expected = {{1, 1, 0, 0, 1}, //
                                               {1, 1, 0, 0, 1}, //
                                               {1, 1, 1, 1, 1}, //
                                               {1, 0, 0, 0, 0}, //
                                               {0, 1, 1, 0, 0}, //
                                               {0, 0, 0, 0, 1}};


  auto out = mln::morpho::experimental::closing_by_reconstruction(ima, markers, mln::experimental::c4);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}
