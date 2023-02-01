#include "segment.hpp"

#include "process/tracker.hpp"

namespace scribo::internal
{
  Segment::Segment(Tracker&& tracker, int nb_to_remove)
  {
    int last_index = static_cast<int>(tracker.impl->n_values.size()) - nb_to_remove;

    spans = tracker.impl->segment_spans;
    spans.reserve(spans.size() + last_index);
    for (int i = 0; i < last_index; i++)
    {
      Span span      = Span();
      span.x         = tracker.impl->t_values[i];
      span.y         = tracker.impl->n_values[i];
      span.thickness = tracker.impl->thicknesses[i];
      spans.push_back(span);
    }

    under_other_object = tracker.impl->under_other;

    first_span = spans[0];
    last_span  = spans[spans.size() - 1];

    is_horizontal = true;

    length    = last_span.x - first_span.x + 1;
    nb_pixels = 0;
    for (auto& span : spans)
      nb_pixels += span.thickness;
  }
} // namespace scribo::internal