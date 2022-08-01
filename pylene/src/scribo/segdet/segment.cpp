#include "segment.hpp"

#include "process/filter.hpp"

namespace scribo::internal
{
  Segment::Segment(Filter&& filter, int nb_to_remove)
  {
    int last_index = static_cast<int>(filter.impl->n_values.size()) - nb_to_remove;

    spans = filter.impl->segment_spans;
    spans.reserve(spans.size() + last_index);
    for (int i = 0; i < last_index; i++)
    {
      Span span      = Span();
      span.x         = filter.impl->t_values[i];
      span.y         = filter.impl->n_values[i];
      span.thickness = filter.impl->thicknesses[i];
      spans.push_back(span);
    }

    under_other_object = filter.impl->under_other;

    first_span = spans[0];
    last_span  = spans[spans.size() - 1];

    is_horizontal = true;

    length    = last_span.x - first_span.x + 1;
    nb_pixels = 0;
    for (auto& span : spans)
      nb_pixels += span.thickness;
  }
} // namespace scribo::internal