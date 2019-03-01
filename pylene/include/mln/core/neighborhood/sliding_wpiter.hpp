#pragma once

#include <mln/core/iterator/iterator_base.hpp>
#include <mln/core/range/range.hpp>


namespace mln
{

  /// \brief Define an iterator over a weighted siteset centered on a site.
  ///
  ///
  /// If you want the iterator to bind the point, use std::ref/std::cref and
  /// proxy iterator for binded iterators
  ///
  /// Note that the siteset is not copied and its lifetime should be longer that
  /// the iterator's one.
  ///
  /// \tparam Point
  /// \tparam SiteSet
  /// \tparam WeightSet
  template <class Point, class SiteSet, class WeightSet>
  struct sliding_wpiter;

  template <class Point, class SiteSet, class WeightSet>
  sliding_wpiter<Point, SiteSet, WeightSet> make_sliding_wpiter(const Point& p, const SiteSet& pset,
                                                                const WeightSet& wset)
  {
    return {p, pset, wset};
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class P, class W>
  struct wpoint
  {

    constexpr const P& point() const { return m_p; }
    constexpr const W& weight() const { return m_w; }

    P m_p;
    W m_w;
  };

  template <class Point, class SiteSet, class WeightSet>
  struct sliding_wpiter
    : iterator_base<sliding_wpiter<Point, SiteSet, WeightSet>,
                    wpoint<typename range_value<SiteSet>::type, typename range_value<WeightSet>::type>,
                    wpoint<typename range_value<SiteSet>::type, typename range_value<WeightSet>::type>>
  {
  private:
    using W = typename range_value<WeightSet>::type;
    using P = typename range_value<SiteSet>::type;

  public:
    sliding_wpiter(const Point& p, const SiteSet& pset, const WeightSet& wset)
      : m_p(p)
      , m_pset_iter(rng::iter(pset))
      , m_wset_iter(rng::iter(wset))
    {
    }

    void init()
    {
      m_pset_iter.init();
      m_wset_iter.init();
    }
    void next()
    {
      m_pset_iter.next();
      m_wset_iter.next();
    }
    bool         finished() const { return m_pset_iter.finished(); }
    wpoint<P, W> dereference() const { return {static_cast<P>(m_p) + *m_pset_iter, *m_wset_iter}; }

  private:
    Point                                          m_p;
    typename range_const_iterator<SiteSet>::type   m_pset_iter;
    typename range_const_iterator<WeightSet>::type m_wset_iter;
  };
} // namespace mln
