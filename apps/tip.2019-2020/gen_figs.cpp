#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/views.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/experimental/gaussian_directional_2d.hpp>


#include <cmath>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::literals;

constexpr double pi()
{
  return std::atan(1) * 4;
}


template <typename T>
class matrix2d
{
public:
  matrix2d()                = default;
  matrix2d(const matrix2d&) = default;
  matrix2d(matrix2d&&)      = default;
  matrix2d& operator=(const matrix2d&) = default;
  matrix2d& operator=(matrix2d&&) = default;

  matrix2d(std::size_t width, std::size_t height)
    : w_(width)
    , h_(height)
    , data_(width * height, T{})
  {
  }

  const T& operator()(std::size_t x, std::size_t y) const
  {
    assert(x >= 0 && x < w_ && y >= 0 && y < h_ && "Index out of bound");
    return data_[y * w_ + x];
  }

  T& operator()(std::size_t x, std::size_t y)
  {
    assert(x >= 0 && x < w_ && y >= 0 && y < h_ && "Index out of bound");
    return data_[y * w_ + x];
  }

  std::size_t width() const { return w_; }
  std::size_t height() const { return h_; }

private:
  std::size_t    w_, h_;
  std::vector<T> data_;
};

template <typename T>
std::ostream& operator<<(std::ostream& oss, const matrix2d<T>& rhs)
{
  for (std::size_t i = 0; i < rhs.height(); ++i)
  {
    oss << "|";
    for (std::size_t j = 0; j < rhs.width() - 1; ++j)
    {
      oss << rhs(i, j) << ", ";
    }
    oss << rhs(i, rhs.width() - 1) << "|\n";
  }

  return oss;
}

matrix2d<double> create_gaussian_mask(std::size_t sigma)
{
  auto size          = static_cast<std::size_t>(std::ceil(3. * sigma));
  auto gaussian_mask = matrix2d<double>((size * 2 + 1), (size * 2 + 1));
  auto sum           = 0.;

  for (int x = -size, l = size + 1; x < l; ++x)
    for (int y = -size; y < l; ++y)
    {
      auto tmp = 1. / (2. * pi() * sigma * sigma) * std::exp(-(x * x + y * y) / (2. * sigma * sigma));
      gaussian_mask(x + size, y + size) = tmp;
      sum += tmp;
    }

  // for (int i = 0; i < gaussian_mask.size(); ++i)
  //  gaussian_mask[i] = gaussian_mask[i] / sum;

  return gaussian_mask;
}

template <typename Ima>
mln::image2d<uint8_t> gaussian_blur(Ima input, std::size_t sigma)
{
  auto gaussian_mask = create_gaussian_mask(sigma);

  auto mask_size = static_cast<int>(gaussian_mask.width());

  mln::image_build_params bp;
  bp.border = 0;

  auto output = mln::image2d<uint8_t>(input.domain(), bp);

  mln::copy(input, output);

  for (int x = mask_size / 2; x < input.domain().width() - mask_size / 2; ++x)
    for (int y = mask_size / 2; y < input.domain().height() - mask_size / 2; ++y)
    {
      double sum = 0.;
      for (int i = x - mask_size / 2; i < x + mask_size / 2 + 1; ++i)
        for (int j = y - mask_size / 2; j < y + mask_size / 2 + 1; ++j)
        {
          sum += input({static_cast<short int>(i), static_cast<short int>(j)}) *
                 gaussian_mask(i - x + mask_size / 2, j - y + mask_size / 2);
        }

      output({x, y}) = sum;
    }

  return output;
}

namespace bg_sub
{
  namespace details
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
  } // namespace details

  constexpr auto filenames_base = details::sva("a.jpg", "b.jpg", "c.jpg", "d.jpg", "e.jpg", "f.jpg", "g.jpg", "h.jpg",
                                               "i.jpg", "j.jpg", "k.jpg", "l.jpg", "m.jpg");

  constexpr auto filenames_bg =
      details::sva("a_bg.jpg", "b_bg.jpg", "c_bg.jpg", "d_bg.jpg", "e_bg.jpg", "f_bg.jpg", "g_bg.jpg", "h_bg.jpg",
                   "i_bg.jpg", "j_bg.jpg", "k_bg.jpg", "l_bg.jpg", "m_bg.jpg");

  constexpr std::string_view file_path        = "images/bg_sub_samples";
  constexpr std::string_view file_path_tmp    = "images/bg_sub_tmp";
  constexpr std::string_view file_path_mosaic = "images/bg_sub_mosaic";

  constexpr auto filenames = details::svap(filenames_base, filenames_bg);

} // namespace bg_sub

void test_deriche_gaussian2d(std::string_view filename, std::string_view filename_bg)
{
  mln::image2d<mln::rgb8> img_color, bg_color;
  std::cout << "deriche gaussian2d" << std::endl;
  std::cout << (std::string{bg_sub::file_path} + '/' + std::string{filename}) << std::endl;
  std::cout << (std::string{bg_sub::file_path} + '/' + std::string{filename_bg}) << std::endl;
  mln::io::imread(std::string{bg_sub::file_path} + '/' + std::string{filename}, img_color);
  mln::io::imread(std::string{bg_sub::file_path} + '/' + std::string{filename_bg}, bg_color);

  auto grayscale = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };

  auto img_grey = mln::view::transform(img_color, grayscale);
  auto bg_grey  = mln::view::transform(bg_color, grayscale);

  mln::io::imsave(img_grey, std::string{bg_sub::file_path_tmp} + '/' + std::string{filename});
  mln::io::imsave(bg_grey, std::string{bg_sub::file_path_tmp} + '/' + std::string{filename_bg});

  constexpr float kLineVerticalSigma   = 10.;
  constexpr float kLineHorizontalSigma = 10.;
  auto bg_blurred = mln::morpho::experimental::gaussian2d(bg_grey, kLineVerticalSigma, kLineHorizontalSigma, 255);

  mln::io::imsave(bg_blurred, std::string{bg_sub::file_path_tmp} + "/blurred_"s + std::string{filename_bg});
}


void test_bg_sub_pipeline(std::string_view filename, std::string_view filename_bg)
{
  // Input Color
  mln::image2d<mln::rgb8> img_color, bg_color;
  mln::io::imread(std::string{bg_sub::file_path} + '/' + std::string{filename}, img_color);
  mln::io::imread(std::string{bg_sub::file_path} + '/' + std::string{filename_bg}, bg_color);

  // GrayScale (view)
  auto grayscale = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };

  auto img_grey = mln::view::transform(img_color, grayscale);
  auto bg_grey  = mln::view::transform(bg_color, grayscale);

  // Gaussian on BG (algo)
  constexpr float kLineVerticalSigma   = 10.;
  constexpr float kLineHorizontalSigma = 10.;
  auto bg_blurred = mln::morpho::experimental::gaussian2d(bg_grey, kLineVerticalSigma, kLineHorizontalSigma, 255);

  // Substract (view)
  using namespace mln::view::ops;
  auto tmp_grey = img_grey - bg_blurred;

  // thresholding (view)
  float threshold       = 150;
  auto  thesholding_fun = [threshold](auto x) -> uint8_t { return (x < threshold) ? 0 : 255; };
  auto  tmp_thresholded = mln::view::transform(tmp_grey, thesholding_fun);

  // erosion (algo)
  mln::se::disc win(3);
  auto          tmp_eroded = mln::morpho::experimental::erosion(tmp_thresholded, win);

  // dilation (algo)
  auto tmp_dilated = mln::morpho::experimental::dilation(tmp_eroded, win);

  // output
  mln::io::imsave(tmp_dilated, std::string{bg_sub::file_path_tmp} + "/result_"s + std::string{filename_bg});
}


void mosaic_fig(std::string_view filename, int mosaic_size)
{
  mln::image2d<mln::rgb8> img_color;
  mln::io::imread(std::string{bg_sub::file_path} + '/' + std::string{filename}, img_color);

  mln::image_build_params bp;
  bp.border     = 15;
  bp.init_value = mln::rgb8{0, 0, 0};

  auto mosaic_domain = mln::box2d{img_color.domain().width() * mosaic_size, img_color.domain().height() * mosaic_size};

  auto mosaic_image = mln::image2d<mln::rgb8>{mosaic_domain, bp};

  for (int w = 0; w < mosaic_size; ++w)
  {
    for (int h = 0; h < mosaic_size; ++h)
    {
      auto tmp_dom = img_color.domain();
      auto tl      = tmp_dom.tl();
      auto br      = tmp_dom.br();
      tl[0] += w * tmp_dom.width();
      tl[1] += h * tmp_dom.height();
      br[0] += w * tmp_dom.width();
      br[1] += h * tmp_dom.height();
      auto sub_dom = mln::box2d{tl, br};
      mln::copy(img_color, mosaic_image.clip(sub_dom));
    }
  }

  mln::io::imsave(mosaic_image, std::string{bg_sub::file_path_mosaic} + "/mosaic_"s + std::to_string(mosaic_size) +
                                    "x" + std::to_string(mosaic_size) + '_' + std::string{filename});
}


int main()
{
  for (auto&& img : bg_sub::filenames)
  {
    test_deriche_gaussian2d(img.first, img.second);
    test_bg_sub_pipeline(img.first, img.second);
    for (int i = 2; i <= 15; ++i)
    {
      mosaic_fig(img.first, i);
      mosaic_fig(img.second, i);
    }
  }

  mln::image2d<mln::rgb8> lena_color;
  mln::io::imread("images/lena_color.png", lena_color);


  mln::io::imsave(mln::view::red(lena_color), "images/lena_red_mono.png");
  mln::io::imsave(mln::view::green(lena_color), "images/lena_green_mono.png");
  mln::io::imsave(mln::view::blue(lena_color), "images/lena_blue_mono.png");

  auto filter_red   = [](auto v) -> mln::rgb8 { return {v[0], 0, 0}; };
  auto filter_green = [](auto v) -> mln::rgb8 { return {0, v[1], 0}; };
  auto filter_blue  = [](auto v) -> mln::rgb8 { return {0, 0, v[2]}; };

  auto lena_red   = mln::view::transform(lena_color, filter_red);
  auto lena_green = mln::view::transform(lena_color, filter_green);
  auto lena_blue  = mln::view::transform(lena_color, filter_blue);

  mln::io::imsave(lena_red, "images/lena_red.png");
  mln::io::imsave(lena_green, "images/lena_green.png");
  mln::io::imsave(lena_blue, "images/lena_blue.png");

  auto lena_green_monoch         = mln::view::green(lena_color);
  auto lena_green_monoch_blurred = gaussian_blur(lena_green_monoch, 3);
  auto lena_green_blurred        = mln::view::transform(lena_green_monoch_blurred, [](auto g) -> mln::rgb8 {
    return {0, g, 0};
  });
  mln::io::imsave(lena_green_blurred, "images/lena_green_blurred.png");

  auto lena_color_gblurred =
      mln::view::transform(lena_green_monoch_blurred, lena_color, [](auto g, auto col) -> mln::rgb8 {
        return {col[0], g, col[2]};
      });
  mln::io::imsave(lena_color_gblurred, "images/lena_color_gblurred.png");

  using namespace mln::view::ops;
  auto lena_rg = lena_red + lena_green;
  auto lena_rb = lena_red + lena_blue;
  auto lena_gb = lena_green + lena_blue;
  mln::io::imsave(lena_rg, "images/lena_rg.png");
  mln::io::imsave(lena_rb, "images/lena_rb.png");
  mln::io::imsave(lena_gb, "images/lena_gb.png");

  if (mln::all_of(lena_color - lena_red == lena_gb))
  {
    std::cout << "rgb - red == green + blue" << std::endl;
  }
  if (mln::all_of(lena_color - lena_green == lena_rb))
  {
    std::cout << "rgb - green == red + blue" << std::endl;
  }
  if (mln::all_of(lena_color - lena_blue == lena_rg))
  {
    std::cout << "rgb - blue == red + green" << std::endl;
  }

  mln::io::imsave(lena_red + lena_green, "images/lena_rg.png");
  mln::io::imsave(lena_red + lena_blue, "images/lena_rb.png");
  mln::io::imsave(lena_green + lena_blue, "images/lena_bg.png");

  mln::image2d<mln::rgb8> planet_color;
  mln::io::imread("images/planet.png", planet_color);

  auto lena_plus_planet = lena_color + planet_color;
  mln::io::imsave(lena_plus_planet, "images/lena_+_planet.png");

  auto out        = lena_color.concretize().set_init_value(mln::rgb8{0, 0, 0}).build();
  auto zipped_ima = mln::view::zip(out, lena_red, lena_blue);

  auto zipped_pixels = zipped_ima.pixels();
  for (auto&& row : mln::ranges::rows(zipped_pixels))
  {
    for (auto&& zpix : row)
    {
      auto&& [oval, rval, bval] = zpix.val();
      oval                      = rval + bval;
    }
  }
  mln::io::imsave(out, "images/lena_rb2.png");

  if (mln::all_of(lena_rb == out))
  {
    std::cout << "zipped out == red + blue" << std::endl;
  }

  {
    float gamma                      = 2.5f;
    auto  gamma_fun                  = [gamma](mln::rgb8 x) -> mln::rgb8 { return 256.f * pow(x / 256.f, 1 / gamma); };
    auto  lena_color_gamma_corrected = mln::view::transform(lena_color, gamma_fun);
    mln::io::imsave(lena_color_gamma_corrected, "images/lena_color_gamma_corrected.png");
  }

  {
    mln::image2d<uint8_t> lena_grey;
    mln::io::imread("images/lena_grey.png", lena_grey);

    float threshold        = 150;
    auto  binary_fun       = [threshold](auto x) -> uint8_t { return (x < threshold) ? 0 : 255; };
    auto  lena_grey_binary = mln::view::transform(lena_grey, binary_fun);
    mln::io::imsave(lena_grey_binary, "images/lena_grey_binary.png");

    float gamma                     = 2.5f;
    auto  gamma_fun                 = [gamma](auto x) -> uint8_t { return 256.f * pow(x / 256.f, 1 / gamma); };
    auto  lena_grey_gamma_corrected = mln::view::transform(lena_grey, gamma_fun);
    mln::io::imsave(lena_grey_gamma_corrected, "images/lena_grey_gamma_corrected.png");
  }

  {
    mln::image2d<mln::rgb8> lena_color;
    mln::io::imread("images/lena_color.png", lena_color);

    auto lena_color_filtered =
        mln::view::filter(lena_color, [](auto v) -> bool { return (v[0] + v[1] + v[2]) % 2 == 0; });
    fill(lena_color_filtered, mln::rgb8{255, 255, 255});
    mln::io::imsave(lena_color, "images/lena_color_filtered.png");
  }


  {
    mln::image2d<uint8_t> lena_grey;
    mln::io::imread("images/lena_grey.png", lena_grey);

    float threshold                 = 150;
    auto  binary_fun                = [threshold](auto x) -> bool { return x < threshold; };
    auto  lena_grey_binary_filtered = mln::view::filter(lena_grey, binary_fun);
    fill(lena_grey_binary_filtered, uint8_t(0));
    mln::io::imsave(lena_grey, "images/lena_grey_binary_filtered.png");
  }

  {
    mln::image2d<uint8_t> lena_grey;
    mln::io::imread("images/lena_grey.png", lena_grey);

    auto dom           = lena_grey.domain();
    auto tl            = mln::point2d{dom.width() / 4, dom.height() / 4};
    auto br            = mln::point2d{dom.width() / 4 + dom.width() / 2, dom.height() / 4 + dom.width() / 2};
    auto sub_dom       = mln::box2d{tl, br};
    auto sub_lena_grey = mln::view::clip(lena_grey, sub_dom);
    mln::io::imsave(sub_lena_grey, "images/lena_grey_clipped.png");

    fill(sub_lena_grey, uint8_t(255));
    mln::io::imsave(lena_grey, "images/lena_grey_minus_clipped.png");
  }

  {
    mln::image2d<mln::rgb8> lena_color;
    mln::io::imread("images/lena_color.png", lena_color);

    auto dom            = lena_color.domain();
    auto tl             = mln::point2d{dom.width() / 4, dom.height() / 4};
    auto br             = mln::point2d{dom.width() / 4 + dom.width() / 2, dom.height() / 4 + dom.width() / 2};
    auto sub_dom        = mln::box2d{tl, br};
    auto sub_lena_color = mln::view::clip(lena_color, sub_dom);
    mln::io::imsave(sub_lena_color, "images/lena_color_clipped.png");

    fill(sub_lena_color, mln::rgb8{0, 0, 0});
    mln::io::imsave(lena_color, "images/lena_color_minus_clipped.png");

    mln::image2d<mln::rgb8> planet_color;
    mln::io::imread("images/planet.png", planet_color);

    auto dom2             = planet_color.domain();
    auto tl2              = mln::point2d{dom2.width() / 4, dom2.height() / 4};
    auto br2              = mln::point2d{dom2.width() / 4 + dom2.width() / 2, dom2.height() / 4 + dom2.width() / 2};
    auto sub_dom2         = mln::box2d{tl2, br2};
    auto sub_planet_color = mln::view::clip(planet_color, sub_dom2);
    mln::io::imsave(sub_planet_color, "images/planet_clipped.png");

    auto sub_planet_color_clone = clone(sub_planet_color);
    fill(planet_color, mln::rgb8{0, 0, 0});
    copy(sub_planet_color_clone, sub_planet_color);
    mln::io::imsave(planet_color, "images/planet_minus_clipped2.png");

    mln::io::imsave(lena_color + planet_color, "images/lena_+_planet_color.png");
  }

  {
    mln::image2d<mln::rgb8> planet_color;
    mln::io::imread("images/planet.png", planet_color);

    auto dom              = planet_color.domain();
    auto tl               = mln::point2d{dom.width() / 4, dom.height() / 4};
    auto br               = mln::point2d{dom.width() / 4 + dom.width() / 2, dom.height() / 4 + dom.width() / 2};
    auto sub_dom          = mln::box2d{tl, br};
    auto sub_planet_color = mln::view::clip(planet_color, sub_dom);

    fill(sub_planet_color, mln::rgb8{0, 0, 0});
    mln::io::imsave(planet_color, "images/planet_color_minus_clipped.png");

    mln::image2d<mln::rgb8> lena_color;
    mln::io::imread("images/lena_color.png", lena_color);

    auto dom2           = lena_color.domain();
    auto tl2            = mln::point2d{dom2.width() / 4, dom2.height() / 4};
    auto br2            = mln::point2d{dom2.width() / 4 + dom2.width() / 2, dom2.height() / 4 + dom2.width() / 2};
    auto sub_dom2       = mln::box2d{tl2, br2};
    auto sub_lena_color = mln::view::clip(lena_color, sub_dom2);

    auto sub_lena_color_clone = clone(sub_lena_color);
    fill(lena_color, mln::rgb8{0, 0, 0});
    copy(sub_lena_color_clone, sub_lena_color);
    mln::io::imsave(lena_color, "images/lena_color_clipped2.png");

    mln::io::imsave(lena_color + planet_color, "images/lena_+_planet_color2.png");
  }

  {
    mln::image2d<uint8_t> lena_grey;
    mln::io::imread("images/lena_grey.png", lena_grey);

    auto lena_grey_masked = mln::view::mask(lena_grey, lena_grey > 150);
    fill(lena_grey_masked, uint8_t(255));

    mln::io::imsave(lena_grey, "images/lena_grey_masked1.png");
  }

  {
    mln::image2d<uint8_t> lena_grey;
    mln::io::imread("images/lena_grey.png", lena_grey);

    auto lena_grey_masked2 = mln::view::mask(lena_grey, lena_grey <= 150);
    fill(lena_grey_masked2, uint8_t(0));

    mln::io::imsave(lena_grey, "images/lena_grey_masked2.png");
  }

  {
    mln::image2d<uint8_t> lena_grey;
    mln::io::imread("images/lena_grey.png", lena_grey);

    auto lena_grey_masked = mln::view::mask(lena_grey, lena_grey > 150);
    fill(lena_grey_masked, uint8_t(255));

    auto lena_grey_masked2 = mln::view::mask(lena_grey, lena_grey <= 150);
    fill(lena_grey_masked2, uint8_t(0));

    mln::io::imsave(lena_grey, "images/lena_grey_binary.png");
  }

  {
    mln::image2d<uint8_t> lena_grey;
    mln::io::imread("images/lena_grey.png", lena_grey);

    auto invert             = [](auto v) -> uint8_t { return 255 - v; };
    auto lena_grey_inverted = mln::view::transform(lena_grey, invert);

    mln::io::imsave(lena_grey_inverted, "images/lena_grey_inverted.png");
  }

  {
    mln::image2d<mln::rgb8> lena_color;
    mln::io::imread("images/lena_color.png", lena_color);

    auto invert              = [](auto v) -> mln::rgb8 { return 255 - v; };
    auto lena_color_inverted = mln::view::transform(lena_color, invert);

    mln::io::imsave(lena_color_inverted, "images/lena_color_inverted.png");
  }
}
