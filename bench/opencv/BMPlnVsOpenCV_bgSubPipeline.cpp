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
                                            "i.jpg", "g.jpg", "k.jpg", "l.jpg", "m.jpg");

constexpr auto filenames_bg =
    detail::sva("a_bg.jpg", "b_bg.jpg", "c_bg.jpg", "d_bg.jpg", "e_bg.jpg", "f_bg.jpg", "g_bg.jpg", "h_bg.jpg",
                "i_bg.jpg", "g_bg.jpg", "k_bg.jpg", "l_bg.jpg", "m_bg.jpg");

constexpr std::string_view file_path     = "images/bg_sub_samples";
constexpr std::string_view file_path_tmp = "images/bg_sub_tmp";

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

  // thresholding (view)
  float threshold       = 150;
  auto  thesholding_fun = [threshold](auto x) -> uint8_t { return (x < threshold) ? 0 : 255; };
  auto  tmp_thresholded = mln::view::transform(tmp_grey, thesholding_fun);

  // erosion (algo)
  auto win        = mln::experimental::se::disc(3);
  auto tmp_eroded = mln::morpho::experimental::erosion(tmp_thresholded, win);

  // dilation (algo)
  mln::morpho::experimental::dilation(tmp_eroded, win, output);
}


void pln_bg_sub_pipe_algos(const mln::experimental::image2d<mln::rgb8>& img_color,
                           const mln::experimental::image2d<mln::rgb8>& bg_color,
                           mln::experimental::image2d<uint8_t>& /*output*/)
{
  // GrayScale (algo)
  mln::experimental::image2d<uint8_t> img_grey, bg_grey;
  mln::resize(img_grey, img_color);
  mln::resize(bg_grey, bg_color);

  {
    auto zipped_images = mln::view::zip(img_grey, img_color);
    auto zipped_pixels = zipped_images.new_pixels();

    for (auto&& row : mln::ranges::rows(zipped_pixels))
    {
      for (auto&& zpix : row)
      {
        auto&& [grey_pix, color_pix] = zpix.val();
        grey_pix                     = 0.2126 * color_pix[0] + 0.7152 * color_pix[1] + 0.0722 * color_pix[2];
      }
    }
  }
  /*
  for (auto&& [grey_pix, color_pix] :
       mln::view::zip(bg_grey.new_pixels(), bg_grey.new_pixels())
           grey_pix.val() = 0.2126 * color_pix.val()[0] + 0.7152 * color_pix.val()[1] + 0.0722 * color_pix.val()[2];

  // Gaussian on BG (algo)
  const float kLineHeight = 5;
  const float kWordWidth = 5; const float kLineVerticalSigma   = (kLineHeight * 0.5f) * 0.1f;
  const float                             kLineHorizontalSigma = (kWordWidth * 0.5f) * 1.f;
  auto bg_blurred = mln::morpho::experimental::gaussian2d(bg_grey, kLineVerticalSigma, kLineHorizontalSigma, 255);

  // Substract (algo)
  using namespace mln::view::ops; mln::experimental::image2d<uint8_t> tmp_grey; mln::resize(tmp_grey, img_grey);

  for (auto&& [ret, img, bg]
      : mln::view::zip(tmp_grey.new_pixels), img_grey.new_pixels(), bg_grey.new_pixels)))
    ret.val() = img.val() - bg.val();

  // thresholding (algo)
  using namespace mln::view::ops;
  mln::experimental::image2d<uint8_t> tmp_thresholded;
  mln::resize(tmp_thresholded, tmp_grey);

  float threshold = 150;
  for (auto&& [ret, input] : mln::view::zip(tmp_thresholded.new_pixels(), tmp_grey.new_pixels()))
    ret.val() = (input.val() < threshold) ? 0 : 255;

  // erosion (algo)
  auto win        = mln::experimental::se::disc(3);
  auto tmp_eroded = mln::morpho::experimental::erosion(tmp_thresholded, win);

  // dilation (algo)
  mln::morpho::experimental::dilation(tmp_eroded, win, output);
  */
}

class BMPlnVsOpenCV_BgSubPipeline : public benchmark::Fixture
{
public:
  using image_t     = mln::experimental::image2d<mln::rgb8>;
  using out_image_t = mln::experimental::image2d<uint8_t>;

  const std::string filepath =
      std::string{fixtures::ImagePath::get_image_path()} + "../apps/tip.2019-2020/images/bg_sub_samples";

  BMPlnVsOpenCV_BgSubPipeline()
  {
    if (!g_loaded)
    {
      {
        auto it_imgs = g_input_imgs.begin();
        auto it_bgs  = g_input_imgs.begin();
        for (auto&& filename : filenames)
        {
          mln::io::experimental::imread(filepath + '/' + std::string{filename.first}, *it_imgs++);
          mln::io::experimental::imread(filepath + '/' + std::string{filename.second}, *it_bgs++);
        }
      }

      g_loaded = true;
    }

    m_input_imgs = g_input_imgs;
    m_input_bgs  = g_input_bgs;
    {
      m_size       = 0;
      auto it_imgs = g_input_imgs.begin();
      for (auto&& output : m_outputs)
        mln::resize(output, *it_imgs);
      m_size += it_imgs->width() * it_imgs->height();
      ++it_imgs;
    }
  }

  void run(benchmark::State&                                                                           st,
           std::function<void(const image_t& input_img, const image_t& input_bg, out_image_t& output)> callback)
  {
    for (auto _ : st)
      callback(m_input_imgs, m_input_bgs, m_outputs);
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }

protected:
  static bool                                               g_loaded;
  static std::vector<mln::experimental::image2d<mln::rgb8>> g_input_imgs;
  static std::vector<mln::experimental::image2d<mln::rgb8>> g_input_bgs;
  std::vector<mln::experimental::image2d<mln::rgb8>>        m_input_imgs;
  std::vector<mln::experimental::image2d<mln::rgb8>>        m_input_bgs;
  std::vector<mln::experimental::image2d<mln::rgb8>>        m_outputs;
  std::size_t                                               m_size;
};
bool                                               BMPlnVsOpenCV_BgSubPipeline::g_loaded = false;
std::vector<mln::experimental::image2d<mln::rgb8>> BMPlnVsOpenCV_BgSubPipeline::g_input_imgs;
std::vector<mln::experimental::image2d<mln::rgb8>> BMPlnVsOpenCV_BgSubPipeline::g_input_bgs;


// PLN

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_BgSubPipeline, Pln_PipeViews)(benchmark::State& st)
{
  auto f = [](const image_t& input_imgs, const image_t& input_bgs, out_image_t& outputs) {
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
  auto f = [](const image_t& input_imgs, const image_t& input_bgs, out_image_t& outputs) {
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
