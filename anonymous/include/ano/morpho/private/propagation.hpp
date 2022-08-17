#pragma once

#include <ano/core/algorithm/fill.hpp>
#include <ano/core/algorithm/transform.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/extension/border_management.hpp>
#include <ano/core/extension/fill.hpp>
#include <ano/core/neighborhood/c4.hpp>
#include <ano/core/neighborhood/c6.hpp>
#include <ano/core/trace.hpp>

#include <ano/morpho/private/pset.hpp>


namespace ano::morpho::details
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
  [[gnu::noinline]] auto to_infsup(I inf, I sup)
  {
    ano_entering("ano::morpho::details::to_infsup");
    using V                            = image_value_t<I>;
    image_ch_value_t<I, irange<V>> out = imchvalue<irange<V>>(inf);
    ano::transform(inf, sup, out, [](V a, V b) -> irange<V> { return {a, b}; });
    return out;
  }


  template <class I>
  std::vector<image_value_t<I>> //
  propagation(I inf, I sup, image_ch_value_t<I, int> ord, image_point_t<I> pstart, int& max_depth)
  {
    ano_entering("ano::morpho::details::propagation");

    using P = image_point_t<I>;
    using V = image_value_t<I>;

    static_assert(P::ndim == 2 || P::ndim == 3, "Invalid number of dimension");
    assert(inf.domain() == sup.domain());
    assert(inf.domain() == ord.domain());
    assert(inf.domain().has(pstart));

    using connectivity_t = std::conditional_t<P::ndim == 2, ano::c4_t, ano::c6_t>;

    enum
    {
      UNPROCESSED = -1,
      PROCESSED   = 0
    };

    // Ord will hold the depth of the pixel during
    // the propagation, i.e. when it is poped from the queue
    connectivity_t nbh;

    if (!ano::extension::fit(ord, nbh))
      throw std::runtime_error("Image extension is not wide enough");

    ano::fill(ord, UNPROCESSED);
    ano::extension::try_fill(ord, PROCESSED);

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

    int depth = 0;
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
          else if (has_nbh_at_level)
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


} // namespace ano::morpho::details
