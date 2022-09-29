#include <scribo/private/segdet_internal.hpp>

#include <cmath>
#include <utility>

#include "detect_line.hpp"

namespace scribo::internal
{
  using namespace mln;

  void add_span_to_output(segdet_output& out, const Span& span, const int& id, const bool& is_horizontal)
  {
    out.seg_ids.push_back(id);
    // TODO if thickness is even, change midpoint position to reflect it
    out.mid_pos_x.push_back(static_cast<float>(span.x));
    out.mid_pos_y.push_back(static_cast<float>(span.y));
    out.thickness.push_back(span.thickness);
    out.angle.push_back(is_horizontal);
  }

  segdet_output segments_to_output(std::vector<Segment> res)
  {
    auto out = segdet_output();

    int ids = 0;

    for (const auto& seg : res)
    {
      size_t i = 0;
      size_t j = 0;

      while (i < seg.spans.size() && j < seg.under_other_object.size())
      {
        Span span;

        if ((seg.is_horizontal && (seg.spans[i].x < seg.under_other_object[j].x)) ||
            (!seg.is_horizontal && (seg.spans[i].y < seg.under_other_object[j].y)))
        {
          span = seg.spans[i];
          i++;
        }
        else
        {
          span = seg.under_other_object[j];
          j++;
        }
        add_span_to_output(out, span, ids, seg.is_horizontal);
      }
      while (i < seg.spans.size())
      {
        add_span_to_output(out, seg.spans[i], ids, seg.is_horizontal);
        i++;
      }
      while (j < seg.under_other_object.size())
      {
        add_span_to_output(out, seg.under_other_object[j], ids, seg.is_horizontal);
        j++;
      }
      ids++;
    }
    return out;
  }

  SegDetParams adapt_parameters_to_image(image2d<std::uint8_t> image)
  {
    auto params          = SegDetParams();
    int  max_dim         = std::max(image.height(), image.width());
    params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
    return params;
  }

  segdet_output detect_line_span(const image2d<std::uint8_t>& image, int min_len, const SegDetParams& params)
  {
    return segments_to_output(detect_line(std::move(image), min_len, params));
  }

  segdet_output detect_line_span(const image2d<std::uint8_t>& image, int min_len)
  {
    auto params = adapt_parameters_to_image(image);
    return detect_line_span(std::move(image), min_len, params);
  }
} // namespace scribo::internal
