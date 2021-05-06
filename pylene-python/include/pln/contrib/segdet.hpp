#pragma once
#include <map>
#include <mln/contrib/segdet/segdet.hpp>
#include <mln/contrib/segdet/segment.hpp>
#include <mln/core/image_format.hpp>
#include <pybind11/iostream.h>
#include <utility>


using sorry_edwin =
    std::vector<std::pair<bool, std::pair<std::vector<std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>>>>;
namespace pln::contrib::segdet
{
  sorry_edwin detect_line_hat(mln::ndbuffer_image img, int min_len, int disc,
                              std::map<std::string, double> params = std::map<std::string, double>())
  {
    mln::contrib::segdet::Parameters params_cpp;
    if (params.empty())
      params_cpp = mln::contrib::segdet::Parameters();
    else
      params_cpp = mln::contrib::segdet::Parameters(params);

    auto p = mln::contrib::segdet::detect_line(img, min_len, disc, params_cpp);

    sorry_edwin out;

    for (auto& seg : p)
    {
      std::vector<std::vector<uint32_t>> points;
      for (auto& point : seg.points)
        points.push_back(std::vector<uint32_t>({point.x, point.y, point.thickness}));
      std::vector<std::vector<uint32_t>> under;

      auto inner_pair = std::make_pair(points, under);
      auto outer_pair = std::make_pair(seg.is_horizontal, inner_pair);
      out.push_back(outer_pair);
    }

    return out;
  }

} // namespace pln::contrib::segdet