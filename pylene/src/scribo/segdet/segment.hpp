#pragma once

#include "process/tracker.hpp"
#include <scribo/private/span.hpp>

#include <vector>

namespace scribo::internal
{
  struct Segment
  {
    bool is_horizontal;

    std::vector<Span> spans;
    std::vector<Span> under_other_object;

    Span first_span;
    Span last_span;

    int length;
    int nb_pixels;

    Segment(Tracker&& tracker, int nb_to_remove);
  };
} // namespace scribo::internal
