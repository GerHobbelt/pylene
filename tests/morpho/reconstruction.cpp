#include <mln/morpho/reconstruction.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>


#include <gtest/gtest.h>


TEST(Morpho, opening_by_reconstruction_binary)
{
  mln::image2d<bool> ima = {{0, 1, 0, 0, 1}, //
                                          {0, 1, 0, 0, 1}, //
                                          {0, 1, 1, 1, 1}, //
                                          {1, 0, 0, 0, 0}, //
                                          {0, 1, 1, 0, 0}, //
                                          {0, 0, 0, 0, 1}};

  mln::image2d<bool> markers = {{0, 0, 0, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}};

  mln::image2d<bool> expected = {{0, 1, 0, 0, 1}, //
                                               {0, 1, 0, 0, 1}, //
                                               {0, 1, 1, 1, 1}, //
                                               {0, 0, 0, 0, 0}, //
                                               {0, 1, 1, 0, 0}, //
                                               {0, 0, 0, 0, 0}};
  auto out = mln::morpho::opening_by_reconstruction(ima, markers, mln::c4);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}

TEST(Morpho, opening_by_reconstruction_binary_c8)
{
  mln::image2d<bool> ima = {{0, 1, 0, 0, 1}, //
                                          {0, 1, 0, 0, 1}, //
                                          {0, 1, 1, 1, 1}, //
                                          {1, 0, 0, 0, 0}, //
                                          {0, 1, 1, 0, 0}, //
                                          {0, 0, 0, 0, 1}};

  mln::image2d<bool> markers = {{0, 0, 0, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}, //
                                              {0, 0, 1, 0, 0}, //
                                              {0, 0, 0, 0, 0}};

  mln::image2d<bool> expected = {{0, 1, 0, 0, 1}, //
                                               {0, 1, 0, 0, 1}, //
                                               {0, 1, 1, 1, 1}, //
                                               {1, 0, 0, 0, 0}, //
                                               {0, 1, 1, 0, 0}, //
                                               {0, 0, 0, 0, 0}};
  auto out = mln::morpho::opening_by_reconstruction(ima, markers, mln::c8);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}



TEST(Morpho, opening_by_reconstruction_gray)
{
  mln::image2d<uint8_t> ima = {{0, 10, +0, +0, 11}, //
                                             {0, 11, +0, +0, 12}, //
                                             {0, 12, 13, 14, 13}, //
                                             {1, +0, +0, +0, +0}, //
                                             {0, +1, +1, +0, +0}, //
                                             {0, +0, +0, +0, +1}};

  mln::image2d<uint8_t> markers = {{0, 0, 0, 0, 0},  //
                                                 {0, 0, 0, 0, 0},  //
                                                 {0, 12, 0, 0, 0}, //
                                                 {0, 0, 0, 0, 0},  //
                                                 {0, 0, 1, 0, 0},  //
                                                 {0, 0, 0, 0, 0}};

  mln::image2d<uint8_t> expected = {{0, 10, 0, 0, 11},   //
                                                  {0, 11, 0, 0, 12},   //
                                                  {0, 12, 12, 12, 12}, //
                                                  {0, 0, 0, 0, 0},     //
                                                  {0, 1, 1, 0, 0},     //
                                                  {0, 0, 0, 0, 0}};

  auto out = mln::morpho::opening_by_reconstruction(ima, markers, mln::c4);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}

TEST(Morpho, closing_by_reconstruction_binary)
{
  mln::image2d<bool> ima = {{0, 1, 0, 0, 1}, //
                                          {0, 1, 0, 0, 1}, //
                                          {0, 1, 1, 1, 1}, //
                                          {1, 0, 0, 0, 0}, //
                                          {0, 1, 1, 0, 0}, //
                                          {0, 0, 0, 0, 1}};

  mln::image2d<bool> markers = {{1, 1, 1, 0, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {1, 1, 1, 1, 1}, //
                                              {0, 1, 1, 1, 1}};

  mln::image2d<bool> expected = {{1, 1, 0, 0, 1}, //
                                               {1, 1, 0, 0, 1}, //
                                               {1, 1, 1, 1, 1}, //
                                               {1, 0, 0, 0, 0}, //
                                               {0, 1, 1, 0, 0}, //
                                               {0, 0, 0, 0, 1}};


  auto out = mln::morpho::closing_by_reconstruction(ima, markers, mln::c4);

  ASSERT_IMAGES_EQ_EXP(out, expected);
}
