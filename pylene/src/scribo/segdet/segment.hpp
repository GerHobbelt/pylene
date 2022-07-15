#pragma once

#include <scribo/private/span.hpp>
#include "process/filter.hpp"

#include <vector>

namespace scribo::internal
{
  struct Segment
  {
    bool is_horizontal;

    std::vector<Span> spans;
    std::vector<Span> under_other_object;

    Span  first_span;
    Span  last_span;
    float first_part_slope;
    float last_part_slope;

    int length;
    int nb_pixels;

    Segment(Filter&& filter, int nb_to_remove);
  };
} // namespace scribo::internal
