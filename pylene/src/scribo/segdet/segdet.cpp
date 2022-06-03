#include "detect_line.hpp"
#include "label_array.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/io/imsave.hpp>
#include <scribo/segdet.hpp>

#include <cmath>
#include <utility>

namespace scribo::internal
{
  void detect_line_label(mln::image2d<std::uint8_t> image, int min_len, mln::image2d<std::uint16_t> out,
                         const SegDetParams& params)
  {
    // Preprocessing
    image2d<std::uint8_t> preprocessed_img = preprocess(std::move(image), params);

    // Processing
    int  min_len_embryo = min_len / 4 + 1; // 5U < min_len ? 5U : min_len;
    auto p              = process(preprocessed_img, min_len_embryo, params);

    // Post Processing

    auto res = post_process(p.first, p.second, preprocessed_img.size(0), preprocessed_img.size(1), min_len, params);
    mln::fill(out, 0);

    labeled_arr(out, res);
  }

  std::vector<Segment> detect_line_seg(const image2d<std::uint8_t>& image, int min_len, const SegDetParams& params)
  {
    // Preprocessing
    image2d<std::uint8_t> preprocessed_img = preprocess(std::move(image), params);

    // Processing
    int  min_len_embryo = min_len / 4 + 1; // 5U < min_len ? 5U : min_len;
    auto p              = process(preprocessed_img, min_len_embryo, params);

    // Post Processing
    auto result = post_process(p.first, p.second, preprocessed_img.size(0), preprocessed_img.size(1), min_len, params);
    return result;
  }
} // namespace scribo::internal

namespace scribo
{
  using namespace internal;

  void add_span_to_output(segdet_output& out, const Span& span, const int& id, const bool& is_horizontal)
  {
    out.seg_ids.push_back(id);
    // TODO if thickness is even, change midpoint position to reflect it
    out.mid_pos_x.push_back(static_cast<float>(span.x));
    out.mid_pos_y.push_back(static_cast<float>(span.y));
    out.thickness.push_back(span.thickness);
    auto slope = is_horizontal ? -span.angle : -((span.angle >= 0) - (span.angle < 0)) * (90 - std::abs(span.angle));
    if (slope == -90)
      slope = 90;
    if (slope == -0)
      slope = 0;
    out.angle.push_back(slope); // TODO FIXME
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

  // Public functions
  segdet_output detect_line(const image2d<std::uint8_t>& image, int min_len, const SegDetParams& params)
  {
    return segments_to_output(detect_line_seg(std::move(image), min_len, params));
  }

  segdet_output detect_line(const mln::image2d<std::uint8_t>& image, int min_len)
  {
    auto params = adapt_parameters_to_image(image);
    return detect_line(std::move(image), min_len, params);
  }
} // namespace scribo