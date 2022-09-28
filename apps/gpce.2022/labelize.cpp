#include <ano/core/algorithm/copy.hpp>
#include <ano/core/algorithm/fill.hpp>
#include <ano/core/algorithm/for_each.hpp>
#include <ano/core/colors.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/image/ndimage.hpp>
#include <ano/core/image/view/filter.hpp>
#include <ano/core/image/view/rgb.hpp>
#include <ano/core/image/view/transform.hpp>
#include <ano/core/image/view/zip.hpp>
#include <ano/core/neighborhood/c8.hpp>
#include <ano/io/imread.hpp>
#include <ano/io/imsave.hpp>
#include <ano/labeling/blobs.hpp>


#include <fmt/core.h>


#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

std::unordered_map<int, unsigned> count_labels(const ano::image2d<uint8_t>& ima_labels)
{
  auto label_count = std::unordered_map<int, unsigned>{};

  {
    auto&& vals = ima_labels.values();
    for (auto&& row : ano::ranges::rows(vals))
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

  ano::image2d<uint8_t> input;
  ano::io::imread(path + "images/input.pgm", input);

  // threshold input
  uint8_t threshold    = 125;
  auto    thresholding = [threshold](auto v) -> uint8_t { return v > threshold ? 255 : 0; };

  auto input_thresholded = ano::view::transform(input, thresholding);

  ano::io::imsave(input_thresholded, path + "images/input_thresholded.pgm");

  // perform connected components labeling
  int  nlabel;
  auto ima_labels = ano::labeling::blobs<uint8_t>(input_thresholded, ano::c8, nlabel);
  fmt::print("nlabels = {}\n", nlabel);
  ano::io::imsave(ima_labels, path + "images/labels.pgm");

  // count each label
  auto label_count = count_labels(ima_labels);

  for (auto [k, v] : label_count)
  {
    fmt::print("{} -> {}\n", k, v);
  }

  // build an output image with color channel to see result properly with fill
  ano::image2d<ano::rgb8> colored_output = ano::imchvalue<ano::rgb8>(input).set_init_value(ano::rgb8{0, 0, 0}).build();

  // copy original thresholded uint8_t image into colored rgb8 images :
  {
    auto imas = ano::view::zip(input_thresholded, colored_output);
    auto vals = imas.values();
    for (auto&& row : ano::ranges::rows(vals))
    {
      for (auto&& v : row)
      {
        auto&& [in, out] = v;
        out              = ano::rgb8{in, in, in};
      }
    }
  }

  // build a labelized image from labels & output
  auto ima_labelized = ano::view::zip(colored_output, ima_labels);

  // filter labelized by number of connected component
  auto min_comps    = 600u;
  auto max_comps    = 4500u;
  auto ima_filtered = ano::view::filter(ima_labelized, [&](auto v) {
    auto&& [val, lab] = v;
    return label_count[lab] > min_comps && label_count[lab] < max_comps;
  });

  auto red = ano::rgb8{255, 0, 0};
  ano::for_each(ima_filtered, [&](auto&& val) {
    auto&& [v_rgb, v_lbl] = val; // ignore label here
    v_rgb                 = red; // assign color to red
  });

  // ano::fill(ima_filtered, std::make_tuple(ano::rgb8{255, 0, 0}, 0u));

  ano::io::imsave(colored_output, path + "images/output.pgm");
}
