#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/transform.hpp>
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
  std::vector<image_value_t<I>> //
  propagation(I inf, I sup, image_ch_value_t<I, int> out, image_point_t<I> pstart, int& max_depth);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class V>
  struct irange
  {
    V inf;
    V sup;
  };


  template <class I>
  [[gnu::noinline]]
  auto to_infsup(I inf, I sup)
  {
    mln_entering("mln::morpho::details::to_infsup");
    using V = image_value_t<I>;
    image_ch_value_t<I, irange<V>> out = imchvalue<irange<V>>(inf);
    mln::transform(inf, sup, out, [](V a, V b) -> irange<V> { return {a, b}; });
    return out;
  }


  template <class I>
  std::vector<image_value_t<I>> //
  propagation(I inf, I sup, image_ch_value_t<I, int> ord, image_point_t<I> pstart, int& max_depth)
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


    auto F = to_infsup(inf, sup);

    pset<I>        queue(inf);
    std::vector<V> depth2lvl;

    auto p              = pstart;
    V    previous_level = inf(p);
    queue.insert(previous_level, p);

    // if (compute_indexes)
    //   sorted_indexes->push_back(p);

    int  depth    = 0;
    depth2lvl.push_back(previous_level);

    while (!queue.empty())
    {
      auto [cur_level, p] = queue.pop(previous_level);
      if (cur_level != previous_level)
      {
        ++depth;
        depth2lvl.push_back(cur_level);
      }

      // if (compute_indexes)
      //   sorted_indexes->push_back(p);

      bool has_nbh_at_level;
      while (true)
      {

        P nextp;
        has_nbh_at_level = false;
        ord(p)           = depth;

        for (auto q : nbh(p))
        {
          if (ord.at(q) != UNPROCESSED)
            continue;

          auto [m, M] = F(q);
          if (M < cur_level)
            queue.insert(M, q);
          else if (cur_level < m)
            queue.insert(m, q);
          else  if (has_nbh_at_level)
            queue.insert(cur_level, q);
          else
          {
            has_nbh_at_level = true;
            nextp            = q;
            continue;
          }
          ord(q) = PROCESSED;
        }

        // If propagation stops at this point
        if (!has_nbh_at_level)
          break;

        p = nextp;
      }

      previous_level = cur_level;
    }
    max_depth = depth;
    return depth2lvl;
  }


} // namespace mln::moprho::experimental::details

