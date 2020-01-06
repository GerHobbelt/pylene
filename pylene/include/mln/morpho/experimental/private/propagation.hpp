#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c6.hpp>

#include <mln/morpho/experimental/private/pset.hpp>


namespace mln::morpho::experimental::details
{
  /// Propagation
  template <class I>
  void propagation(I inf, I sup, image_ch_value_t<I, int> out, image_point_t<I> pstart, int& max_depth);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  void propagation(I inf, I sup, image_ch_value_t<I, int> ord, image_point_t<I> pstart, int& max_depth)
  {
    mln_entering("mln::morpho::details::propagation");

    using P              = image_point_t<I>;
    using V              = image_value_t<I>;


    static_assert(P::ndim == 2 || P::ndim == 3, "Invalid number of dimension");
    assert(inf.domain() == sup.domain());
    assert(inf.domain() == ord.domain());
    assert(inf.domain().has(pstart));

    using connectivity_t = std::conditional_t<P::ndim == 2, mln::experimental::c4_t, mln::experimental::c6_t>;

    enum
    {
      UNPROCESSED = -1,
      PROCESSED   = 0
    };

    // Ord will hold the depth of the pixel during
    // the propagation, i.e. when it is poped from the queue
    connectivity_t nbh;

    if (!mln::extension::fit(ord, nbh))
      throw std::runtime_error("Image extension is not wide enough");

    mln::fill(ord, UNPROCESSED);
    mln::extension::try_fill(ord, PROCESSED);

    // if (compute_indexes)
    //   sorted_indexes->reserve(ord.domain().size());

    pset<I> queue(inf);

    auto p              = pstart;
    V    previous_level = inf(p);
    queue.insert(previous_level, p);
    ord(p) = 0;

    // if (compute_indexes)
    //   sorted_indexes->push_back(p);

    int  depth    = 0;
    while (!queue.empty())
    {
      V cur_level;
      P p;

      auto tmp = queue.try_pop(previous_level);
      if (tmp.has_value())
      {
        cur_level = previous_level;
        p         = tmp.value();
      }
      else
      {
        std::tie(cur_level, p) = queue.pop(previous_level);
        ++depth;
      }
      ord(p) = depth;

      // if (compute_indexes)
      //   sorted_indexes->push_back(p);

      for (auto q : nbh(p))
      {
        if (ord.at(q) == UNPROCESSED)
        {
          auto m = inf(q);
          auto M = sup(q);
          if (M < cur_level)
            queue.insert(M, q);
          else if (cur_level < m)
            queue.insert(m, q);
          else
            queue.insert(cur_level, q);
          ord(q) = PROCESSED;
        }
      }
      previous_level = cur_level;
    }
    max_depth = depth;
  }


} // namespace mln::moprho::experimental::details

