#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/private/ndbuffer_image.hpp>
#include <mln/core/image_format.hpp>
#include <scribo/private/segdet_internal.hpp>
#include <scribo/private/segment.hpp>

#include <pln/core/image_cast.hpp>
#include <pybind11/pybind11.h>

#include <exception>
#include <utility>

#include "segdet.hpp"

#define GET_VARIABLE_NAME(Variable) (#Variable)

namespace pln::scribo
{

  ::scribo::SegDetParams parse_parameters(const std::map<std::string, float>& map)
  {
    auto params = ::scribo::SegDetParams();
    for (auto& kvp : map)
    {
      auto str = kvp.first;
      auto val = kvp.second;

      if (str == GET_VARIABLE_NAME(nb_values_to_keep))
        params.nb_values_to_keep = val;
      else if (str == GET_VARIABLE_NAME(dyn))
        params.dyn = val;
      else if (str == GET_VARIABLE_NAME(default_sigma_position))
        params.default_sigma_position = val;
      else if (str == GET_VARIABLE_NAME(default_sigma_thickness))
        params.default_sigma_thickness = val;
      else if (str == GET_VARIABLE_NAME(default_sigma_luminosity))
        params.default_sigma_luminosity = val;
      else if (str == GET_VARIABLE_NAME(min_nb_values_sigma))
        params.min_nb_values_sigma = val;
      else if (str == GET_VARIABLE_NAME(sigma_pos_min))
        params.sigma_pos_min = val;
      else if (str == GET_VARIABLE_NAME(sigma_thickness_min))
        params.sigma_thickness_min = val;
      else if (str == GET_VARIABLE_NAME(sigma_luminosity_min))
        params.sigma_luminosity_min = val;
      else if (str == GET_VARIABLE_NAME(max_thickness))
        params.max_thickness = val;
      else if (str == GET_VARIABLE_NAME(max_llum))
        params.max_llum = val;
      else if (str == GET_VARIABLE_NAME(ratio_lum))
        params.ratio_lum = val;
      else if (str == GET_VARIABLE_NAME(merge_slope_variation))
        params.merge_slope_variation = val;
      else if (str == GET_VARIABLE_NAME(merge_distance_max))
        params.merge_distance_max = val;
      else if (str == GET_VARIABLE_NAME(threshold_intersection))
        params.threshold_intersection = val;
      else if (str == GET_VARIABLE_NAME(minimum_for_fusion))
        params.minimum_for_fusion = val;
      else if (str == GET_VARIABLE_NAME(discontinuity))
        params.discontinuity = val;
      else
        exit(-255);
    }
    return params;
  }

  ::scribo::SegDetParams get_parameters(const std::map<std::string, float>& params)
  {
    ::scribo::SegDetParams params_cpp;
    if (params.empty())
      params_cpp = ::scribo::SegDetParams();
    else
      params_cpp = parse_parameters(params);
    if (!params_cpp.is_valid())
    {
      throw std::invalid_argument("One of the parameters is invalid.");
    }

    return params_cpp;
  }

  std::vector<::scribo::internal::Segment> detect_line_base(mln::ndbuffer_image img, int min_len,
                                                            const std::map<std::string, float>& params)
  {
    ::scribo::SegDetParams params_cpp = get_parameters(params);
    auto*                  cast_img   = img.cast_to<std::uint8_t, 2>();
    if (!cast_img)
      throw std::invalid_argument("Input image should be 2D uint8");
    return ::scribo::internal::detect_line_seg(*cast_img, min_len, params_cpp);
  }


  mln::ndbuffer_image detect_line_pixel(mln::ndbuffer_image img, int min_len,
                                        const std::map<std::string, float>& params)
  {
    auto segments = detect_line_base(std::move(img), min_len, params);

    std::vector<std::vector<int>> segment_pixels = {};

    int x, y;
    int width = img.size(0), height = img.size(1);

    int segments_number = static_cast<int>(segments.size());
    for (int s = 0; s < segments_number; s++)
    {
      for (const auto& span : segments[s].spans)
      {
        int thickness = static_cast<int>(span.thickness);
        for (int t = -thickness / 2 - 1; t < ((thickness / 2) + (thickness % 2) + 1); t++)
        {
          x = static_cast<int>(span.x);
          y = static_cast<int>(span.y);

          if (segments[s].is_horizontal)
          {
            y += t;

            if (y < 0 || height <= y)
              continue;
          }
          else
          {
            x += t;

            if (x < 0 || width <= x)
              continue;
          }

          segment_pixels.push_back({s + 1, x, y});
        }
      }
    }

    mln::image2d<std::uint32_t> out(3, static_cast<int>(segment_pixels.size()));

    int out_count = 0;
    for (const auto& segment_pixel : segment_pixels)
    {
      out({0, out_count})   = segment_pixel[0];
      out({1, out_count})   = segment_pixel[1];
      out({2, out_count++}) = segment_pixel[2];
    }

    return static_cast<mln::ndbuffer_image>(out);
  }

  std::vector<::scribo::internal::Span> douglas_peucker(const std::vector<::scribo::internal::Span>& spans,
                                                        float                                        precision)
  {
    if (spans.size() < 2)
      return spans;

    float a = (float)((float)spans.back().y - (float)spans.front().y) /
              (float)((float)spans.back().x - (float)spans.front().x);
    float b = (float)spans.front().y - a * (float)spans.front().x;

    size_t xmax = 1;
    float  dmax = 0;

    for (size_t x = 1; x < spans.size(); x++)
    {
      auto  px = spans[x];
      float d  = std::abs((float)px.y - a * (float)px.x - b) / sqrt(1 + a * a);

      if (d > dmax)
      {
        dmax = d;
        xmax = x;
      }
    }

    if (dmax < precision)
      return {spans.front()};

    auto left  = douglas_peucker(std::vector<::scribo::internal::Span>(spans.begin(), spans.begin() + xmax), precision);
    auto right = douglas_peucker(std::vector<::scribo::internal::Span>(spans.begin() + xmax, spans.end()), precision);

    left.insert(left.end(), right.begin(), right.end());

    return left;
  }

  void swapXY(std::vector<::scribo::internal::Span>& spans)
  {
    for (auto& span : spans)
      std::swap(span.x, span.y);
  }

  std::vector<::scribo::internal::Span> init_douglas_peucker(std::vector<::scribo::internal::Span>& spans,
                                                             bool is_horizontal, float precision)
  {
    using namespace ::scribo;
    if (!is_horizontal)
      swapXY(spans);

    std::sort(spans.begin(), spans.end(), [](internal::Span& lhs, internal::Span& rhs) { return lhs.x > rhs.x; });

    auto last   = spans.back();
    auto output = douglas_peucker(spans, precision);
    output.push_back(last);

    if (!is_horizontal)
      swapXY(output);

    return output;
  }

  mln::ndbuffer_image detect_line_vectors(mln::ndbuffer_image img, int min_len,
                                          const std::map<std::string, float>& params, float precision = 15)
  {
    std::vector<::scribo::internal::Segment> p = detect_line_base(std::move(img), min_len, params);

    std::vector<std::vector<::scribo::internal::Span>> vector_spans = {};
    int                                                nb_spans     = 0;

    for (auto& segment : p)
    {
      std::vector<::scribo::internal::Span> spans =
          init_douglas_peucker(segment.spans, segment.is_horizontal, precision);
      nb_spans += static_cast<int>(spans.size());
      vector_spans.push_back(spans);
    }

    mln::image2d<std::uint32_t> out(3, nb_spans);
    int                         id_seg = 0;
    int                         count  = 0;
    for (auto& spans : vector_spans)
    {
      for (auto& span : spans)
      {
        out({0, count})   = id_seg;
        out({1, count})   = span.x;
        out({2, count++}) = span.y;
      }

      id_seg++;
    }

    return static_cast<mln::ndbuffer_image>(out);
  }

  mln::ndbuffer_image detect_line_vector(mln::ndbuffer_image img, int min_len,
                                         const std::map<std::string, float>& params)
  {
    std::vector<::scribo::internal::Segment> segments = detect_line_base(std::move(img), min_len, params);

    int                         segments_number = static_cast<int>(segments.size());
    mln::image2d<std::uint32_t> out(4, segments_number);

    for (int i = 0; i < segments_number; i++)
    {
      std::vector<::scribo::internal::Span> spans =
          init_douglas_peucker(segments[i].spans, segments[i].is_horizontal, 50000);
      out({0, i}) = spans[0].x;
      out({1, i}) = spans[0].y;
      out({2, i}) = spans[1].x;
      out({3, i}) = spans[1].y;
    }

    return static_cast<mln::ndbuffer_image>(out);
  }

  pybind11::array detect_line_label(mln::ndbuffer_image img, int min_len,
                                        const std::map<std::string, float>& params)
  {
    mln::image2d<uint16_t> out(img.size(0), img.size(1));
    auto*                  cast_img   = img.cast_to<std::uint8_t, 2>();
    if (!cast_img)
      throw std::invalid_argument("Input image should be 2D uint8");

    ::scribo::internal::detect_line_label(*cast_img, min_len, out, parse_parameters(params));


    return to_numpy(out);
  }

  pybind11::array detect_line(mln::ndbuffer_image img, int min_len, const std::string& mode,
                                  const std::map<std::string, float>& params)
  {
    if (mode == "pixel")
      return to_numpy(detect_line_pixel(img, min_len, params));
    else if (mode == "vector")
      return to_numpy(detect_line_vector(img, min_len, params));
    else if (mode == "polyline")
      return to_numpy(detect_line_vectors(img, min_len, params));
    else
      throw std::invalid_argument(R"(Invalid mode. Suitable modes are "pixel", "vector", "polyline".)");
  }
} // namespace pln::scribo