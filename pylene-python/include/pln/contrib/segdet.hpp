#pragma once
#include <exception>
#include <map>
#include <mln/contrib/segdet/segdet.hpp>
#include <mln/contrib/segdet/segment.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image_format.hpp>
#include <pybind11/iostream.h>
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

  std::vector<mln::contrib::segdet::Point> douglas_peucker(const std::vector<mln::contrib::segdet::Point>& points,
                                                           double                                          precision)
  {
    if (points.size() < 2)
      return points;

    double a = (double) ((double)points.back().y - (double) points.front().y) / (double) ((double)points.back().x - (double) points.front().x);
    double b = (double) points.front().y - a * (double) points.front().x;

    size_t    xmax = 1;
    double dmax = 0;

    for (size_t x = 1; x < points.size(); x++)
    {
      auto   px = points[x];
      double d  = std::abs((double) px.y - a * (double) px.x - b) / sqrt(1 + a * a);

      if (d > dmax)
      {
        dmax = d;
        xmax = x;
      }
    }

    if (dmax < precision)
      return {points.front()};

    auto left =
        douglas_peucker(std::vector<mln::contrib::segdet::Point>(points.begin(), points.begin() + xmax), precision);
    auto right =
        douglas_peucker(std::vector<mln::contrib::segdet::Point>(points.begin() + xmax, points.end()), precision);

    left.insert(left.end(), right.begin(), right.end());

    return left;
  }

  void swapXY(std::vector<mln::contrib::segdet::Point>& points)
  {
    for (auto& point : points)
    {
      auto tmp = point.x;
      point.x  = point.y;
      point.y  = tmp;
    }
  }

  std::vector<mln::contrib::segdet::Point> init_douglas_peucker(std::vector<mln::contrib::segdet::Point>& points,
                                                                bool is_horizontal, double precision)
  {
    using namespace mln::contrib::segdet;
    if (!is_horizontal)
      swapXY(points);

    std::sort(points.begin(), points.end(), [](Point& lhs, Point& rhs){ return lhs.x > rhs.x;});
    
    auto last   = points.back();
    pybind11::print(last.x, last.y);
    auto output = douglas_peucker(points, precision);
    output.push_back(last);

    if (!is_horizontal)
      swapXY(output);

    return output;
  }


  mln::ndbuffer_image
  detect_line_vectorize(mln::ndbuffer_image img, int min_len, int disc,
                        const std::map<std::string, double>& params    = std::map<std::string, double>(),
                        double                               precision = 15)
  {
    std::vector<mln::contrib::segdet::Segment> p = detect_line_base(std::move(img), min_len, disc, params);

    std::vector<std::vector<mln::contrib::segdet::Point>> vector_points = {};
    int                                                   nb_points     = 0;

    for (auto& segment : p)
    {
      std::vector<mln::contrib::segdet::Point> points =
          init_douglas_peucker(segment.points, segment.is_horizontal, precision);
      nb_points += points.size();
      vector_points.push_back(points);
    }

    mln::image2d<uint32_t> out(3, nb_points);
    int                    id_seg = 0;
    int                    count  = 0;
    for (auto& points : vector_points)
    {
      for (auto& point : points)
      {
        out({0, count})   = id_seg;
        out({1, count})   = point.x;
        out({2, count++}) = point.y;
      }

      id_seg++;
    }

    return std::move(out);
  }
} // namespace pln::contrib::segdet