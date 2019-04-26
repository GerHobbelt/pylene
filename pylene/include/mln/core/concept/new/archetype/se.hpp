#pragma once

#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/neighborhood.hpp>
#include <mln/core/neighborhood/neighborhood_traits.hpp>

#include <range/v3/iterator_range.hpp>


namespace mln::archetypes
{
  // This file defines:
  // StructuringElementT<P = Point, Pix = Pixel<P>>
  // DecomposableStructuringElement<P = Point, Pix = Pixel<P>>

  namespace details
  {

    template <class P, class Pix>
#ifdef PYLENE_CONCEPT_TS_ENABLED
    requires mln::concepts::Point<P>&& mln::concepts::Pixel<Pix>
#endif
    struct StructuringElement
    {
      using category     = adaptative_neighborhood_tag;
      using incremental  = std::false_type;
      using decomposable = std::false_type;
      using separable    = std::false_type;

      ::ranges::iterator_range<P*> operator()(P p);
      ::ranges::iterator_range<P*> before(P p);
      ::ranges::iterator_range<P*> after(P p);

      ::ranges::iterator_range<Pix*> operator()(Pix px);
      ::ranges::iterator_range<Pix*> before(Pix px);
      ::ranges::iterator_range<Pix*> after(Pix px);
    };


    template <class SE>
    struct AsSE : SE, mln::experimental::StructuringElement<AsSE<SE>>
    {
    };
  } // namespace details

  template <class P = Point, class Pix = PixelT<P>>
  using StructuringElement = details::AsSE<details::StructuringElement<P, Pix>>;


  namespace details
  {
    template <class P, class Pix>
    struct DecomposableStructuringElement : StructuringElement<P, Pix>
    {
      using decomposable = std::true_type;

      bool                                                                   is_decomposable() const;
      ::ranges::iterator_range<mln::archetypes::StructuringElement<P, Pix>*> decompose() const;
    };

    template <class P, class Pix>
    struct SeparableStructuringElement : StructuringElement<P, Pix>
    {
      using separable = std::true_type;

      bool                                                                   is_separable() const;
      ::ranges::iterator_range<mln::archetypes::StructuringElement<P, Pix>*> separate() const;
    };

    template <class P, class Pix>
    struct IncrementalStructuringElement : StructuringElement<P, Pix>
    {
      using incremental = std::true_type;

      archetypes::StructuringElement<P, Pix> inc() const;
      archetypes::StructuringElement<P, Pix> dec() const;
    };
  } // namespace details


  template <class P = Point, class Pix = PixelT<P>>
  using DecomposableStructuringElement = details::AsSE<details::DecomposableStructuringElement<P, Pix>>;

  template <class P = Point, class Pix = PixelT<P>>
  using SeparableStructuringElement = details::AsSE<details::SeparableStructuringElement<P, Pix>>;

  template <class P = Point, class Pix = PixelT<P>>
  using IncrementalStructuringElement = details::AsSE<details::IncrementalStructuringElement<P, Pix>>;

} // namespace mln::archetypes
