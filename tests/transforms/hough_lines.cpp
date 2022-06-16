#include <gtest/gtest.h>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/transforms/hough_lines.hpp>
#include <cmath>
#include <numbers>

TEST(HoughLines, detect_peaks)
{
  int n=500;
  mln::image2d<bool> in(n,n);
  mln::fill(in, false);
  for (int i = 0; i < n; ++i)
    in({i, i}) = true;

  for (int i = 0; i < n-10; ++i)
  {
    in({10 + i, i}) = true;
    in({i, 10 + i}) = true;
  }

  // -50° <-> -40° in radians (100 angles)
  float angles[] = {
      -0.87266463f, -0.8709193f,  -0.86917397f, -0.86742864f, -0.86568331f, -0.86393798f, -0.86219265f, -0.86044732f,
      -0.85870199f, -0.85695666f, -0.85521133f, -0.853466f,   -0.85172067f, -0.84997535f, -0.84823002f, -0.84648469f,
      -0.84473936f, -0.84299403f, -0.8412487f,  -0.83950337f, -0.83775804f, -0.83601271f, -0.83426738f, -0.83252205f,
      -0.83077672f, -0.82903139f, -0.82728607f, -0.82554074f, -0.82379541f, -0.82205008f, -0.82030475f, -0.81855942f,
      -0.81681409f, -0.81506876f, -0.81332343f, -0.8115781f,  -0.80983277f, -0.80808744f, -0.80634211f, -0.80459679f,
      -0.80285146f, -0.80110613f, -0.7993608f,  -0.79761547f, -0.79587014f, -0.79412481f, -0.79237948f, -0.79063415f,
      -0.78888882f, -0.78714349f, -0.78539816f, -0.78365283f, -0.7819075f,  -0.78016218f, -0.77841685f, -0.77667152f,
      -0.77492619f, -0.77318086f, -0.77143553f, -0.7696902f,  -0.76794487f, -0.76619954f, -0.76445421f, -0.76270888f,
      -0.76096355f, -0.75921822f, -0.7574729f,  -0.75572757f, -0.75398224f, -0.75223691f, -0.75049158f, -0.74874625f,
      -0.74700092f, -0.74525559f, -0.74351026f, -0.74176493f, -0.7400196f,  -0.73827427f, -0.73652894f, -0.73478362f,
      -0.73303829f, -0.73129296f, -0.72954763f, -0.7278023f,  -0.72605697f, -0.72431164f, -0.72256631f, -0.72082098f,
      -0.71907565f, -0.71733032f, -0.71558499f, -0.71383966f, -0.71209433f, -0.71034901f, -0.70860368f, -0.70685835f,
      -0.70511302f, -0.70336769f, -0.70162236f, -0.69987703f, -0.6981317f};


  auto ht = mln::transforms::hough_lines(in, angles);
  auto peaks = mln::transforms::hough_lines_detect_peaks(ht, angles, 0.5f, 2, 5);

  EXPECT_NEAR(peaks[0].angle, -std::numbers::pi_v<float> / 4, 0.1f * std::numbers::pi_v<float> / 180);
  EXPECT_FLOAT_EQ(peaks[0].radius, 0);
  EXPECT_FLOAT_EQ(peaks[0].count, n);

  EXPECT_NEAR(peaks[1].angle, -std::numbers::pi_v<float> / 4, 0.1f * std::numbers::pi_v<float> / 180);
  EXPECT_FLOAT_EQ(peaks[1].radius, std::round(10 * std::numbers::sqrt2_v<float> / 2));
  EXPECT_FLOAT_EQ(peaks[1].count, n - 10);

  EXPECT_NEAR(peaks[2].angle, -std::numbers::pi_v<float> / 4, 0.1f * std::numbers::pi_v<float> / 180);
  EXPECT_FLOAT_EQ(peaks[2].radius, -std::round(10 * std::numbers::sqrt2_v<float> / 2));
  EXPECT_FLOAT_EQ(peaks[2].count, n - 10);
}

TEST(HoughLines, detect_peaks_p90)
{
  int                n = 500;
  int                k = 50;
  mln::image2d<bool> in(n, n, mln::image_build_params{.init_value = false});
  for (int i = 0; i < n; ++i)
  {
    in({i, i}) = true;
    in({i, n - i - 1}) = true;
  }

  for (int i = 0; i < n-k; ++i)
  {
    in({k + i, i})         = true;
    in({i, k + i})         = true;
    in({i + k, n - i - 1}) = true;
    in({i, n - k - i - 1}) = true;
  }


  // -50° <-> -40° in radians (100 angles)
  float angles[] = {
      -0.87266463f, -0.8709193f,  -0.86917397f, -0.86742864f, -0.86568331f, -0.86393798f, -0.86219265f, -0.86044732f,
      -0.85870199f, -0.85695666f, -0.85521133f, -0.853466f,   -0.85172067f, -0.84997535f, -0.84823002f, -0.84648469f,
      -0.84473936f, -0.84299403f, -0.8412487f,  -0.83950337f, -0.83775804f, -0.83601271f, -0.83426738f, -0.83252205f,
      -0.83077672f, -0.82903139f, -0.82728607f, -0.82554074f, -0.82379541f, -0.82205008f, -0.82030475f, -0.81855942f,
      -0.81681409f, -0.81506876f, -0.81332343f, -0.8115781f,  -0.80983277f, -0.80808744f, -0.80634211f, -0.80459679f,
      -0.80285146f, -0.80110613f, -0.7993608f,  -0.79761547f, -0.79587014f, -0.79412481f, -0.79237948f, -0.79063415f,
      -0.78888882f, -0.78714349f, -0.78539816f, -0.78365283f, -0.7819075f,  -0.78016218f, -0.77841685f, -0.77667152f,
      -0.77492619f, -0.77318086f, -0.77143553f, -0.7696902f,  -0.76794487f, -0.76619954f, -0.76445421f, -0.76270888f,
      -0.76096355f, -0.75921822f, -0.7574729f,  -0.75572757f, -0.75398224f, -0.75223691f, -0.75049158f, -0.74874625f,
      -0.74700092f, -0.74525559f, -0.74351026f, -0.74176493f, -0.7400196f,  -0.73827427f, -0.73652894f, -0.73478362f,
      -0.73303829f, -0.73129296f, -0.72954763f, -0.7278023f,  -0.72605697f, -0.72431164f, -0.72256631f, -0.72082098f,
      -0.71907565f, -0.71733032f, -0.71558499f, -0.71383966f, -0.71209433f, -0.71034901f, -0.70860368f, -0.70685835f,
      -0.70511302f, -0.70336769f, -0.70162236f, -0.69987703f, -0.6981317f};



  auto ht = mln::transforms::hough_lines(in, angles);
  std::ranges::transform(angles, angles, [](float v) { return v + 0.5f * std::numbers::pi_v<float>; });
  ht = mln::transforms::hough_lines(in, angles, &ht);

  auto peaks = mln::transforms::hough_lines_detect_peak_angles(ht, angles, 0.5f,  5, 5);

  EXPECT_NEAR(peaks[0].angle, std::numbers::pi_v<float> / 4, 0.1f * std::numbers::pi_v<float> / 180);
  auto expected = 2 * (n + n - k + n - k);
  EXPECT_NEAR(peaks[0].count, expected, 0.01f * expected);
}

