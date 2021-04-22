#pragma once
#include <mln/contrib/segdet/segdet.hpp>
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
    auto img2d = img.__cast<mln::rgb8, 2>();
    mln::image2d<uint8_t> out =  mln::contrib::segdet::preprocess_img(std::move(img2d));
    return std::move(out);
  }

} // namespace pln::contrib::segdet