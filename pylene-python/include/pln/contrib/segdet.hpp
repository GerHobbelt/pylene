#pragma once
#include <map>
#include <mln/contrib/segdet/segdet.hpp>
#include <mln/contrib/segdet/segment.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image_format.hpp>
#include <pybind11/iostream.h>
#include <exception>
#include <utility>

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
    if (!params_cpp.is_valid())
    {
      throw std::invalid_argument("One of the parameters is invalid.");
    }

    return mln::contrib::segdet::detect_line(std::move(img), min_len, disc, params_cpp);
  }

  mln::ndbuffer_image
  detect_line_labelled(mln::ndbuffer_image img, int min_len, int disc,
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
    for (auto& seg : p)
      nb_points += seg.nb_pixels;

    mln::image2d<uint32_t> out(3, nb_points);

    int count  = 0;
    int nb_seg = 0;
    for (auto& seg : p)
    {
      for (auto& point : seg.points)
      {
        out({0, count})   = nb_seg;
        out({1, count})   = point.x;
        out({2, count++}) = point.y;
      }

      nb_seg++;
    }

    return std::move(out);
  }
} // namespace pln::contrib::segdet