#pragma once

#include "process/tracker.hpp"
#include <scribo/private/span.hpp>

#include <vector>

namespace scribo::internal
{
  /**
   * @brief A segment is a set of spans that are connected together.
   *
   */
  struct Segment
  {
    bool is_horizontal; /** True if the segment is horizontal, false otherwise. */

    std::vector<Span> spans;              /** The spans that compose the segment. */
    std::vector<Span> under_other_object; /** The spans that are under other objects. */

    Span first_span; /** The first span of the segment. */
    Span last_span;  /** The last span of the segment. */

    int length;    /** The length of the segment in pixels (the number of pixels in the x direction if horizontal and in
                      the y direction if vertical). */
    int nb_pixels; /** The number of pixels of the segment. */

    /**
     * @brief Construct a new Segment object
     *
     * @param tracker The tracker that will be used to construct the segment.
     * @param nb_to_remove The number of spans to remove from the end of the tracker (useful the creation of the segment
     * is due to a fusion).
     */
    Segment(Tracker&& tracker, int nb_to_remove);
  };
} // namespace scribo::internal
