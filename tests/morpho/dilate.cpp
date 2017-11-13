#include <mln/core/algorithm/fill.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/structural/dilate.hpp>

#include <tests/helpers.hpp>

#include <gtest/gtest.h>

using namespace mln;

TEST(Morpho, dilation_dilate_0)
{
    image2d<uint8> ima;
    io::imread(MLN_IMG_PATH "small.pgm", ima);

    { // Fast: border wide enough
        auto win = make_rectangle2d(7, 7);
        auto out = morpho::structural::dilate(ima, win);
        ASSERT_TRUE(all(out >= ima)); // extensive
    }
}

// Border is not wide enough => use morpher for bound checking
TEST(Morpho, dilation_dilate_1)
{
    image2d<uint8> ima;
    io::imread(MLN_IMG_PATH "small.pgm", ima);

    auto win = make_rectangle2d(9, 9);
    auto out = morpho::structural::dilate(ima, win);
    ASSERT_TRUE(all(out >= ima)); // extensive
}

// Dilation on a with a vmorph / binary case
TEST(Morpho, dilation_dilate_2)
{
    image2d<uint8> ima;
    io::imread(MLN_IMG_PATH "small.pgm", ima);

    auto win = make_rectangle2d(3, 3);
    auto out = morpho::structural::dilate(ima > 128, win);
    ASSERT_TRUE(all(out >= (ima > 128))); // extensive
}

// Dilation on a with a vmorph / binary case
TEST(Morpho, dilation_dilate_view)
{
    image2d<uint8> ima;
    io::imread(MLN_IMG_PATH "small.pgm", ima);

    auto win = make_rectangle2d(3, 3);
    auto out = clone(ima);
    auto tmp = out | where(ima > 128);
    morpho::structural::dilate(ima | where(ima > 128), win, tmp, std::less<uint8>());
    ASSERT_TRUE(all((out | (ima <= 128)) == (ima | (ima <= 128))));
    ASSERT_TRUE(all(out >= ima)); // extensive
}

// Custom comparison function, erosion
TEST(Morpho, dilation_dilate_with_custom_cmp_function)
{
    image2d<uint8> ima;
    io::imread(MLN_IMG_PATH "small.pgm", ima);

    auto win = make_rectangle2d(5, 5);
    auto out = morpho::structural::dilate(ima, win, std::greater<uint8>());
    ASSERT_TRUE(all(out <= ima)); // anti-extensive
}

// Dilation of a binary image
TEST(Morpho, dilation_dilate_binary)
{
    image2d<bool> ima(11, 11);
    fill(ima, false);
    ima.at(0, 0) = true;
    ima.at(5, 5) = true;
    ima.at(10, 10) = true;

    auto win = make_rectangle2d(3, 3);
    auto out = morpho::structural::dilate(ima, win);
    ASSERT_TRUE(all(ima <= out)); // anti-extensive
}

// Dilation of a bianry image
TEST(Morpho, dilation_dilate_binary_2)
{
    image2d<bool> ima;
    io::imread(MLN_IMG_PATH "tiny.pbm", ima);

    auto win = make_rectangle2d(3, 3);
    auto out = morpho::structural::dilate(ima, win);
    ASSERT_TRUE(all(ima <= out)); // anti-extensive
}

// Dilation of a rgb image
TEST(Morpho, dilation_dilate_rgb)
{
    image2d<rgb8> ima;
    io::imread(MLN_IMG_PATH "small.ppm", ima);

    auto win = make_rectangle2d(5, 5);
    auto out = morpho::structural::dilate(ima, win);
    ASSERT_TRUE(all(red(ima) <= red(out)));     // anti-extensive
    ASSERT_TRUE(all(green(ima) <= green(out))); // anti-extensive
    ASSERT_TRUE(all(blue(ima) <= blue(out)));   // anti-extensive
}
