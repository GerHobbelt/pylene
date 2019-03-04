#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/iterator/iterator_base.hpp>
#include <mln/core/neighborhood/sliding_pixter.hpp>
#include <mln/core/range/range.hpp>


namespace mln
{

  /// \brief Define a pixel iterator over a weighted siteset centered on a pixel.
  ///
  /// Define an iterator over a weighted siteset centered on a pixel.
  /// If you want the iterator to bind the point, use std::ref/std::cref and
  /// proxy iterator for binded iterators
  ///
  ///
  /// Note that the siteset is not copied and
  /// its lifetime should be longer that the iterator's one.
  ///
  /// Internally, the iterator is optimized so that:
  /// * if I is raw, it uses pointers
  /// * if I is indexable, it uses indexes
  /// * otherwise, it uses points.
  ///
  /// \tparam Pixel
  /// \tparam SiteSet
  /// \tparam WeightSet
  /// \pre The pixel's image must be accessible or indexable.
  template <class Pixel, class SiteSet, class WeightSet>
  struct sliding_wpixter;

  template <class Pixel, class SiteSet, class WeightSet>
  auto make_sliding_wpixter(const Pixel& px, const SiteSet& pset, const WeightSet& wset)
  {
    return sliding_wpixter<Pixel, SiteSet, WeightSet>(px, pset, wset);
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  /// \FIXME bad (very bad!)
  /// The exact type is wrong. SCOOP is broken for implementing
  /// mixin.
  template <class Px, class W>
  struct wpixel : Px
  {
    constexpr wpixel() = default;
    constexpr wpixel(const Px& px, const W& w)
      : Px(px)
      , m_w(w)
    {
    }
    constexpr const W& weight() const { return m_w; }

  private:
    W m_w;
  };

  template <class Pixel, class SiteSet, class WeightSet>
  struct sliding_wpixter : iterator_base<sliding_wpixter<Pixel, SiteSet, WeightSet>,
                                         wpixel<typename iterator_traits<sliding_pixter<Pixel, SiteSet>>::value_type,
                                                typename range_value<WeightSet>::type>,
                                         wpixel<typename iterator_traits<sliding_pixter<Pixel, SiteSet>>::value_type,
                                                typename range_value<WeightSet>::type>>
  {
  private:
    using Px = typename iterator_traits<sliding_pixter<Pixel, SiteSet>>::value_type;
    using W  = typename range_value<WeightSet>::type;

  public:
    sliding_wpixter() = default;

    sliding_wpixter(const Pixel& px, const SiteSet& pset, const WeightSet& wset)
      : m_sliding_pixter(px, pset)
      , m_wset_iter(rng::iter(wset))
    {
    }

    void init()
    {
      m_sliding_pixter.init();
      m_wset_iter.init();
    }

    void next()
    {
      m_sliding_pixter.next();
      m_wset_iter.next();
    }

    bool finished() const { return m_sliding_pixter.finished(); }

    wpixel<Px, W> dereference() const { return {*m_sliding_pixter, *m_wset_iter}; }

  private:
    sliding_pixter<Pixel, SiteSet>                 m_sliding_pixter;
    typename range_const_iterator<WeightSet>::type m_wset_iter;
  };

} // namespace mln
