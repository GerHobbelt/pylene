#pragma once
#include <map>
#include <mln/contrib/segdet/segdet.hpp>
#include <mln/contrib/segdet/segment.hpp>
#include <mln/core/image_format.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <pybind11/iostream.h>
#include <utility>


using sorry_edwin =
    std::vector<std::pair<bool, std::pair<std::vector<std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>>>>;

namespace pln::contrib::segdet
{

  std::vector<mln::contrib::segdet::Segment> detect_line_base(mln::ndbuffer_image img, int min_len, int disc,
                                                              const std::map<std::string, double>& params)
  {
    mln::contrib::segdet::Parameters params_cpp;
    if (params.empty())
      params_cpp = mln::contrib::segdet::Parameters();
    else
      params_cpp = mln::contrib::segdet::Parameters(params);

    return mln::contrib::segdet::detect_line(std::move(img), min_len, disc, params_cpp);
  }

  sorry_edwin detect_line_sorry_edwin(mln::ndbuffer_image img, int min_len, int disc,
                    const std::map<std::string, double>& params = std::map<std::string, double>())
  {
    auto p = detect_line_base(img, min_len, disc, params);
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

  mln::ndbuffer_image detect_line_labelled(mln::ndbuffer_image img, int min_len, int disc,
                                           const std::map<std::string, double>& params = std::map<std::string, double>())
  {
    auto p = detect_line_base(img, min_len, disc, params);

    mln::image2d<uint16_t> out(img.size(0), img.size(1));
    mln::fill(out, 0);
    mln::contrib::segdet::labeled_arr(out, p);
    return std::move(out);
  }

  mln::ndbuffer_image detect_line_numpy(mln::ndbuffer_image img, int min_len, int disc,
                                        const std::map<std::string, double>& params = std::map<std::string, double>())
  {
    auto p = detect_line_base(std::move(img), min_len, disc, params);

    int nb_points = 0;
    for (auto &seg : p)
    {
      nb_points += seg.nb_pixels;
    }

    mln::image2d<uint32_t> out(3, nb_points);

    int count = 0;
    int nb_seg = 0;
    for (auto& seg : p)
    {
      for (auto& point : seg.points)
      {
        out({0, count}) = nb_seg;
        out({1, count}) = point.x;
        out({2, count++}) = point.y;
      }
      nb_seg++;
    }

    return std::move(out);
  }



} // namespace pln::contrib::segdet