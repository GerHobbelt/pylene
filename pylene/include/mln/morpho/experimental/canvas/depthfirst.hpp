#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/morpho/experimental/private/pqueue.hpp>



namespace  mln::morpho::experimental::canvas
{



  template <class I, class N, class DFVisitor>
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
  // viz.has_level_flooding_started(Level l) : true if this level is already being flooded

  template <class I, class N, class DFVisitor>
  void depthfirst(I& f, N nbh, DFVisitor& viz, image_point_t<I> start)
  {
    enum st
    {
      NONE    = 0,
      INQUEUE = 1,
      DONE    = 255,
    };
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

    constexpr auto impl_type = mln::morpho::experimental::details::pqueue_impl::vector;
    mln::morpho::experimental::details::pqueue_fifo<I, impl_type> queue(f);

    queue.push(f(start), start);
    status(start) = INQUEUE;


    // Flooding function turned non-recursive: flood(p, current_level = f(p))
    {
      P    p             = start;
      auto current_level = f(start);
      int  pstatus;

    flood:
      viz.on_flood_start(current_level, p);
      pstatus = 0;

    keep_flooding:

      {
        int k = 1;
        for (auto n : nbh(p))
        {
          int mask = 1 << k++;
          if ((pstatus & mask) || status.at(n) != NONE)
            continue;

          // Insert n INQUEUE
          auto nval = f(n);
          status(n) = INQUEUE;
          queue.push(nval, n);
          pstatus |= mask;

          // If the neighbor is lower, postpone the neighbor
          if (nval <= current_level)
            continue;

          // Otherwise, process it, (do not remove p from stack)
          status(p)     = pstatus;
          current_level = nval;
          p             = n;
          goto flood;
        }
      }

      // All the neighbors have been seen, p is DONE
      // status(p) = DONE;
      viz.on_done(current_level, p);
      queue.pop();

      // If the queue gets empty, we have processed the whole image
      if (queue.empty())
        goto end_flooding;

      {
        auto old_level = current_level;
        std::tie(current_level, p) = queue.top();
        pstatus                    = status(p);
        if (current_level == old_level)
          goto keep_flooding;
        viz.on_flood_end(old_level, current_level);
      }

      if (viz.has_level_flooding_started(current_level))
        goto keep_flooding;
      else
        goto flood;

      // End: there is no more point to process
    end_flooding:
      viz.on_flood_end(current_level);
    }
  }
}
