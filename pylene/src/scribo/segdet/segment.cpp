
#include "filter.hpp"
#include <scribo/private/segment.hpp>

namespace scribo::internal
{
  Segment::Segment(const Filter& filter, int nb_to_remove)
  {
    int last_index = static_cast<int>(filter.n_values.size()) - nb_to_remove;

    spans = filter.segment_spans;
    for (int i = 0; i < last_index; i++)
    {
      Span span = Span();
      span.x    = filter.t_values[i];
      span.y    = filter.n_values[i];
      span.thickness = filter.thicknesses[i];
      span.angle = std::atan(filter.current_slope) * 180 / 3.14;
      spans.push_back(span);
    }

    under_other_object = filter.under_other;

    first_part_slope = spans[0].angle;
    last_part_slope  = filter.current_slope;

    first_span = spans[0];
    last_span  = spans[spans.size() - 1];

    is_horizontal = true;

    length    = last_span.x - first_span.x + 1;
    nb_pixels = 0;
    for (auto& span : spans)
      nb_pixels += span.thickness;
  }
} // namespace mln::contrib::segdet