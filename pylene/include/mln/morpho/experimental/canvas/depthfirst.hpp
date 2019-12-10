#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/morpho/experimental/private/pqueue.hpp>


#include <range/v3/functional.hpp>
#include <bitset>

namespace  mln::morpho::experimental::canvas
{



  template <class I, class N, class DFVisitor, class Proj = ::ranges::cpp20::identity>
  void depthfirst(I& f, N nbh, DFVisitor& viz, image_point_t<I> start);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  // The visitor must implement
  //
  //
  // viz.on_flood_start(Level l, Point p): called when we start flooding a preak component at level l of p
  //
  // viz.on_flood_end(Level l): called when we end flooding the the peak component
  //
  // viz.on_done(Level l, Point p): called when a point has been processed
  //
  // viz.has_node_at_level(Level l) : true if there is a node at this level in branch

  template <class I, class N, class DFVisitor, class Proj>
  void depthfirst(I& f, N nbh, DFVisitor& viz, image_point_t<I> start)
  {
    enum st { NONE = 0, INQUEUE = 1, DONE = 2 };
    using P = image_point_t<I>;
    //using V = image_value_t<I>;

    // FIXME
    image_build_error_code err = IMAGE_BUILD_OK;

    auto status = imchvalue<uint8_t>(f) //
                      .adjust(nbh)
                      .set_init_value(NONE)
                      .get_status(&err)
                      .build();

    if (err != IMAGE_BUILD_OK)
      throw std::runtime_error("Unable to have an extension.");

    status.extension().fill(DONE);

    mln::morpho::experimental::details::pqueue_fifo<I> queue(f);

    queue.push(f(start), start);
    status(start) = INQUEUE;

    constexpr int nvalues = 1 << 8;
    std::bitset<nvalues> has_level;

    // Flooding function turned non-recursive: flood(p, current_level = f(p))
    {
      P    p             = start;
      auto current_level = f(start);

    flood_new_level:
      has_level.set(current_level);
      viz.on_flood_start(current_level, p);

    flood_flat_zone:
      for (auto n : nbh(p))
      {
        if (status(n) != NONE)
          continue;

        // Insert n INQUEUE
        auto nval = f(n);
        status(n) = INQUEUE;
        queue.push(nval, n);

        // If the neighbor is lower, postpone the neighbor
        if (nval <= current_level)
          continue;

        // Otherwise, process it, (do not remove p from stack)
        current_level = nval;
        p             = n;
        goto flood_new_level;
      }

      // All the neighbors have been seen, p is DONE
      status(p) = DONE;
      viz.on_done(current_level, p);
      queue.pop();

      // If the queue gets empty, we have processed the whole image
      if (!queue.empty())
      {
        auto old_level = current_level;
        std::tie(current_level, p) = queue.top();
        if (current_level == old_level)
          goto flood_flat_zone;

        viz.on_flood_end(old_level);
        has_level.reset(old_level);

        if (has_level.test(current_level))
          goto flood_flat_zone;

        goto flood_new_level;
      }

    // End: there is no more point to process
      viz.on_flood_end(current_level);
    }
  }
}
