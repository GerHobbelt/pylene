#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/channel.hpp>
#include <mln/core/image/view/filter.hpp>
#include <mln/core/image/view/mask.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/gaussian2d.hpp>


#include <fmt/core.h>


#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

std::unordered_map<int, unsigned> count_labels(const mln::image2d<uint8_t>& ima_labels)
{
  auto label_count = std::unordered_map<int, unsigned>{};

  {
    auto&& vals = ima_labels.values();
    for (auto&& row : mln::ranges::rows(vals))
    {
      for (auto&& v : row)
      {
        if (label_count.contains(v))
          ++label_count[v];
        else
          label_count.emplace(v, 1u);
      }
    }
  }
  return label_count;
}

int main(int, char** argv)
{
  // load image
  std::string path = argv[1];

  mln::image2d<uint8_t> input;
  mln::io::imread(path + "images/input.pgm", input);

  // threshold input
  uint8_t threshold    = 125;
  auto    thresholding = [threshold](auto v) -> uint8_t { return v > threshold ? 255 : 0; };

  auto input_thresholded = mln::view::transform(input, thresholding);

  mln::io::imsave(input_thresholded, path + "images/input_thresholded.pgm");

  // perform connected components labeling
  int  nlabel;
  auto ima_labels = mln::labeling::blobs<uint8_t>(input_thresholded, mln::c8, nlabel);
  fmt::print("nlabels = {}\n", nlabel);
  mln::io::imsave(ima_labels, path + "images/labels.pgm");

  // count each label
  auto label_count = count_labels(ima_labels);

  for (auto [k, v] : label_count)
  {
    fmt::print("{} -> {}\n", k, v);
  }

  // build an output image with color channel to see result properly with fill
  mln::image2d<mln::rgb8> colored_output = mln::imchvalue<mln::rgb8>(input).set_init_value(mln::rgb8{0, 0, 0}).build();

  // copy original thresholded uint8_t image into colored rgb8 images :
  {
    auto imas = mln::view::zip(input_thresholded, colored_output);
    auto vals = imas.values();
    for (auto&& row : mln::ranges::rows(vals))
    {
      for (auto&& v : row)
      {
        auto&& [in, out] = v;
        out              = mln::rgb8{in, in, in};
      }
    }
  }

  // build a labelized image from labels & output
  auto ima_labelized = mln::view::zip(colored_output, ima_labels);

  // filter labelized by number of connected component
  auto min_comps    = 600u;
  auto max_comps    = 4500u;
  auto ima_filtered = mln::view::filter(ima_labelized, [&](auto v) {
    auto&& [val, lab] = v;
    return label_count[lab] > min_comps && label_count[lab] < max_comps;
  });

  auto red = mln::rgb8{255, 0, 0};
  mln::for_each(ima_filtered, [&](auto&& val) {
    auto&& [v_rgb, v_lbl] = val; // ignore label here
    v_rgb                 = red; // assign color to red
  });

  // mln::fill(ima_filtered, std::make_tuple(mln::rgb8{255, 0, 0}, 0u));

  mln::io::imsave(colored_output, path + "images/output.pgm");
}
