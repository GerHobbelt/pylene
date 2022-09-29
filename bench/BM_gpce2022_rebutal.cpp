#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/filter.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/morpho/erosion.hpp>
#include <mln/morpho/watershed.hpp>

#include <benchmark/benchmark.h>

#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::literals;

static const std::string g_path = "./apps/icip.2022/images/";

class BMgpce2022_rebutal : public benchmark::Fixture
{
  virtual void SetUp(const benchmark::State&) override
  {
    mln::io::imread(g_path + "connected_component_labels.png", m_input_rgb8);
    m_pixel_count = m_input_rgb8.domain().size();
    m_disc        = mln::se::disc{3};
  }

protected:
  std::size_t             m_pixel_count;
  mln::image2d<mln::rgb8> m_input_rgb8;
  mln::se::disc           m_disc;
};

BENCHMARK_F(BMgpce2022_rebutal, labelize_view)(benchmark::State& st)
{
  while (st.KeepRunning())
  {

    auto grayscale     = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };
    auto ima_grayscale = mln::view::transform(m_input_rgb8, grayscale);

    uint8_t threshold    = 125;
    auto    thresholding = [threshold](auto v) -> uint8_t { return v > threshold ? 255 : 0; };

    auto input_thresholded = mln::view::transform(ima_grayscale, thresholding);

    // perform connected components labeling
    // int  nlabel;
    auto ima_ret = mln::morpho::erosion(input_thresholded, m_disc);
    // auto ima_ret = mln::labeling::blobs<uint8_t>(input_thresholded, mln::c8, nlabel);
    // auto ima_ret = mln::morpho::watershed<int16_t>(input_thresholded, mln::c8, nlabel);

    benchmark::DoNotOptimize(ima_ret);
  }

  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}

BENCHMARK_F(BMgpce2022_rebutal, labelize_serial)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    auto grayscale = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };

    auto ima_grayscale = mln::transform(m_input_rgb8, grayscale);

    uint8_t threshold    = 125;
    auto    thresholding = [threshold](auto v) -> uint8_t { return v > threshold ? 255 : 0; };

    auto input_thresholded = mln::transform(ima_grayscale, thresholding);

    // perform connected components labeling
    // int  nlabel;
    auto ima_ret = mln::morpho::erosion(input_thresholded, m_disc);
    // auto ima_ret = mln::labeling::blobs<uint8_t>(input_thresholded, mln::c8, nlabel);
    //  auto ima_ret = mln::morpho::watershed<int16_t>(input_thresholded, mln::c8, nlabel);
    benchmark::DoNotOptimize(ima_ret);
  }
  st.SetBytesProcessed(st.iterations() * m_pixel_count);
}


BENCHMARK_MAIN();
