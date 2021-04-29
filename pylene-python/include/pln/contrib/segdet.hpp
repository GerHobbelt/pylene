#pragma once
#include <mln/contrib/segdet/segdet.hpp>
#include <mln/core/image_format.hpp>
#include <pybind11/iostream.h>
#include <utility>


using sorry_edwin =
    std::vector<std::pair<bool, std::pair<std::vector<std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>>>>;
namespace pln::contrib::segdet
{
  sorry_edwin detect_line_hat(mln::ndbuffer_image img, int min_len, int disc)
  {
    auto p = mln::contrib::segdet::detect_line(img.__cast<uint8_t, 2>(), min_len, disc);

    sorry_edwin out;

    for (auto& seg : p)
    {
      std::vector<std::vector<uint32_t>> points;
      for (auto& point : seg.points)
        points.push_back(std::vector<uint32_t>({point.x, point.y, point.thickness}));
      std::vector<std::vector<uint32_t>> under;
      for (auto& under_o : seg.under_other)
        points.push_back(std::vector<uint32_t>({under_o.x, under_o.y, under_o.thickness}));

      auto inner_pair = std::make_pair(points, under);
      auto outer_pair = std::make_pair(seg.is_horizontal, inner_pair);
      out.push_back(outer_pair);
    }

    return out;
  }

  mln::ndbuffer_image preprocess(mln::ndbuffer_image img)
  {
    mln::image2d<uint8_t> out;

    switch (img.sample_type())
    {
    case mln::sample_type_id::UINT8:
      pybind11::print("uint8");
      out        = mln::contrib::segdet::preprocess_img_grayscale(img.__cast<uint8_t, 2>());
      break;
    case mln::sample_type_id::RGB8:
      pybind11::print("rgb8");
      out        = mln::contrib::segdet::preprocess_img_rgb(img.__cast<mln::rgb8, 2>());
      break;
    default:
      pybind11::print("Image type not rgb8 or uint8");
    }

    return std::move(out);
  }

} // namespace pln::contrib::segdet