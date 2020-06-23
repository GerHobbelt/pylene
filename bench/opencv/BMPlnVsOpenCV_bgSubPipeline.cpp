#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/views.hpp>
#include <mln/core/range/view/zip.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/experimental/gaussian_directional_2d.hpp>

#include <fixtures/ImagePath/image_path.hpp>


#include <benchmark/benchmark.h>
#include <opencv2/opencv.hpp>

#include <array>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


using namespace std::literals;


namespace detail
{
  template <size_t N>
  constexpr std::string_view sv(const char (&literal)[N])
  {
    return std::string_view(literal, N - 1);
  }

  template <size_t... N>
  constexpr std::array<std::string_view, sizeof...(N)> sva(const char (&... literals)[N])
  {
    return {{sv(literals)...}};
  }

  template <size_t... Idx>
  constexpr std::array<std::pair<std::string_view, std::string_view>, sizeof...(Idx)>
  swap_impl(std::array<std::string_view, sizeof...(Idx)> la, std::array<std::string_view, sizeof...(Idx)> ra,
            std::index_sequence<Idx...>)
  {
    return {std::make_pair(la[Idx], ra[Idx])...};
  }

  template <size_t N, size_t M>
  constexpr std::array<std::pair<std::string_view, std::string_view>, N> svap(std::array<std::string_view, N> la,
                                                                              std::array<std::string_view, M> ra)
  {
    static_assert(N == M, "Wrong size");
    using Indexes = std::make_index_sequence<N>;
    return swap_impl(la, ra, Indexes{});
  }
} // namespace detail

constexpr auto filenames_base = detail::sva("a.jpg", "b.jpg", "c.jpg", "d.jpg", "e.jpg", "f.jpg", "g.jpg", "h.jpg",
                                            "i.jpg", "j.jpg", "k.jpg", "l.jpg", "m.jpg");

constexpr auto filenames_bg =
    detail::sva("a_bg.jpg", "b_bg.jpg", "c_bg.jpg", "d_bg.jpg", "e_bg.jpg", "f_bg.jpg", "g_bg.jpg", "h_bg.jpg",
                "i_bg.jpg", "j_bg.jpg", "k_bg.jpg", "l_bg.jpg", "m_bg.jpg");

constexpr auto filenames = detail::svap(filenames_base, filenames_bg);


void pln_bg_sub_pipe_views(const mln::experimental::image2d<mln::rgb8>& img_color,
                           const mln::experimental::image2d<mln::rgb8>& bg_color,
                           mln::experimental::image2d<uint8_t>&         output)
{
  // GrayScale (view)
  auto grayscale = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };

  auto img_grey = mln::view::transform(img_color, grayscale);
  auto bg_grey  = mln::view::transform(bg_color, grayscale);

  // Gaussian on BG (algo)
  const float kLineHeight          = 5;
  const float kWordWidth           = 5;
  const float kLineVerticalSigma   = (kLineHeight * 0.5f) * 0.1f;
  const float kLineHorizontalSigma = (kWordWidth * 0.5f) * 1.f;
  auto bg_blurred = mln::morpho::experimental::gaussian2d(bg_grey, kLineVerticalSigma, kLineHorizontalSigma, 255);

  // Substract (view)
  using namespace mln::view::ops;
  auto tmp_grey = img_grey - bg_blurred;

  // Thresholding (view)
  const float threshold       = 150;
  auto        thesholding_fun = [threshold](auto x) -> uint8_t { return (x < threshold) ? 0 : 255; };
  auto        tmp_thresholded = mln::view::transform(tmp_grey, thesholding_fun);

  // Erosion (algo)
  auto win        = mln::experimental::se::disc(3);
  auto tmp_eroded = mln::morpho::experimental::erosion(tmp_thresholded, win);

  // Dilation (algo)
  mln::morpho::experimental::dilation(tmp_eroded, win, output);
}


void pln_bg_sub_pipe_algos(const mln::experimental::image2d<mln::rgb8>& img_color,
                           const mln::experimental::image2d<mln::rgb8>& bg_color,
                           mln::experimental::image2d<uint8_t>&         output)
{
  // GrayScale (algo)
  mln::experimental::image2d<uint8_t> img_grey, bg_grey;
  mln::resize(img_grey, img_color);
  mln::resize(bg_grey, bg_color);
  {
    auto zipped_images = mln::view::zip(img_grey, bg_grey, img_color, bg_color);
    auto zipped_pixels = zipped_images.new_pixels();

    for (auto&& row : mln::ranges::rows(zipped_pixels))
    {
      for (auto&& zpix : row)
      {
        auto&& [img_grey_v, bg_grey_v, img_color_v, bg_color_v] = zpix.val();

        img_grey_v = 0.2126 * img_color_v[0] + 0.7152 * img_color_v[1] + 0.0722 * img_color_v[2];
        bg_grey_v  = 0.2126 * bg_color_v[0] + 0.7152 * bg_color_v[1] + 0.0722 * bg_color_v[2];
      }
    }
  }

  // Gaussian on BG (algo)
  const float kLineHeight          = 5;
  const float kWordWidth           = 5;
  const float kLineVerticalSigma   = (kLineHeight * 0.5f) * 0.1f;
  const float kLineHorizontalSigma = (kWordWidth * 0.5f) * 1.f;
  auto bg_blurred = mln::morpho::experimental::gaussian2d(bg_grey, kLineVerticalSigma, kLineHorizontalSigma, 255);

  // Substract (algo)
  mln::experimental::image2d<uint8_t> tmp_grey;
  mln::resize(tmp_grey, img_grey);
  {
    auto zipped_images = mln::view::zip(tmp_grey, img_grey, bg_grey);
    auto zipped_pixels = zipped_images.new_pixels();

    for (auto&& row : mln::ranges::rows(zipped_pixels))
    {
      for (auto&& zpix : row)
      {
        auto&& [tmp_grey_v, img_grey_v, bg_grey_v] = zpix.val();

        tmp_grey_v = img_grey_v - bg_grey_v;
      }
    }
  }

  // thresholding (algo)
  const float                         threshold = 150;
  mln::experimental::image2d<uint8_t> tmp_thresholded;
  mln::resize(tmp_thresholded, tmp_grey);
  {
    auto zipped_images = mln::view::zip(tmp_thresholded, tmp_grey);
    auto zipped_pixels = zipped_images.new_pixels();

    for (auto&& row : mln::ranges::rows(zipped_pixels))
    {
      for (auto&& zpix : row)
      {
        auto&& [tmp_thresh_v, tmp_grey_v] = zpix.val();

        tmp_thresh_v = (tmp_grey_v < threshold) ? 0 : 255;
      }
    }
  }

  // erosion (algo)
  auto win        = mln::experimental::se::disc(3);
  auto tmp_eroded = mln::morpho::experimental::erosion(tmp_thresholded, win);

  // dilation (algo)
  mln::morpho::experimental::dilation(tmp_eroded, win, output);
}

class BMPlnVsOpenCV_BgSubPipeline : public benchmark::Fixture
{
public:
  using image_t     = mln::experimental::image2d<mln::rgb8>;
  using out_image_t = mln::experimental::image2d<uint8_t>;

  const std::string filepath =
      std::string{fixtures::ImagePath::get_image_path()} + "../apps/tip.2019-2020/images/bg_sub_mosaic";

  BMPlnVsOpenCV_BgSubPipeline()
  {
    if (!g_loaded)
    {
      {
        auto it_imgs = g_input_imgs.begin();
        auto it_bgs  = g_input_bgs.begin();
        auto it_outs = g_outputs.begin();
        for (auto&& filename : filenames)
        {
          mln::io::experimental::imread(filepath + "/mosaic_10x10_" + std::string{filename.first}, *it_imgs);
          mln::io::experimental::imread(filepath + "/mosaic_10x10_" + std::string{filename.second}, *it_bgs);
          mln::resize(*it_outs, *it_imgs);
          g_size += it_imgs->width() * it_imgs->height();
          ++it_imgs;
          ++it_bgs;
          ++it_outs;
        }
      }

      g_loaded = true;
    }

    m_input_imgs = g_input_imgs;
    m_input_bgs  = g_input_bgs;
    m_outputs    = g_outputs;
    m_size       = g_size;
  }

  void run(benchmark::State& st,
           std::function<void(const std::vector<image_t>& input_img, const std::vector<image_t>& input_bg,
                              std::vector<out_image_t>& output)>
               callback)
  {
    for (auto _ : st)
      callback(m_input_imgs, m_input_bgs, m_outputs);
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }

protected:
  static bool                     g_loaded;
  static std::vector<image_t>     g_input_imgs;
  static std::vector<image_t>     g_input_bgs;
  static std::vector<out_image_t> g_outputs;
  static std::size_t              g_size;
  std::vector<image_t>            m_input_imgs;
  std::vector<image_t>            m_input_bgs;
  std::vector<out_image_t>        m_outputs;
  std::size_t                     m_size;
};

bool                                                  BMPlnVsOpenCV_BgSubPipeline::g_loaded = false;
std::size_t                                           BMPlnVsOpenCV_BgSubPipeline::g_size   = 0;
std::vector<BMPlnVsOpenCV_BgSubPipeline::image_t>     BMPlnVsOpenCV_BgSubPipeline::g_input_imgs{filenames.size()};
std::vector<BMPlnVsOpenCV_BgSubPipeline::image_t>     BMPlnVsOpenCV_BgSubPipeline::g_input_bgs{filenames.size()};
std::vector<BMPlnVsOpenCV_BgSubPipeline::out_image_t> BMPlnVsOpenCV_BgSubPipeline::g_outputs{filenames.size()};


// PLN

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeViews)(benchmark::State& st)
{
  auto f = [](const std::vector<image_t>& input_imgs, const std::vector<image_t>& input_bgs,
              std::vector<out_image_t>& outputs) {
    auto it_imgs = input_imgs.begin();
    auto it_bgs  = input_bgs.begin();
    auto it_outs = outputs.begin();
    for (; it_imgs != input_imgs.end(); ++it_imgs, ++it_bgs, ++it_outs)
    {
      pln_bg_sub_pipe_views(*it_imgs, *it_bgs, *it_outs);
    }
  };

  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeAlgos)(benchmark::State& st)
{
  auto f = [](const std::vector<image_t>& input_imgs, const std::vector<image_t>& input_bgs,
              std::vector<out_image_t>& outputs) {
    auto it_imgs = input_imgs.begin();
    auto it_bgs  = input_bgs.begin();
    auto it_outs = outputs.begin();
    for (; it_imgs != input_imgs.end(); ++it_imgs, ++it_bgs, ++it_outs)
    {
      pln_bg_sub_pipe_algos(*it_imgs, *it_bgs, *it_outs);
    }
  };

  this->run(st, f);
}


BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeViews);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeAlgos);

BENCHMARK_MAIN();
