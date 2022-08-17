#pragma once

#include <ano/core/concepts/archetype/pixel.hpp>
#include <ano/core/concepts/archetype/point.hpp>
#include <ano/core/concepts/neighborhood.hpp>

#include <range/v3/view/subrange.hpp>


namespace ano::archetypes
{
  // This file defines:
  // StructuringElementT<P = Point, Pix = Pixel<P>>
  // DecomposableStructuringElement<P = Point, Pix = Pixel<P>>
  // SeparableStructuringElement<P = Point, Pix = Pixel<P>>
  // IncrementalStructuringElement<P = Point, Pix = Pixel<P>>

  namespace details
  {

    template <class P, class Pix>
    // clang-format off
    requires ano::concepts::Point<P>&& ano::concepts::Pixel<Pix>
    struct StructuringElement
    // clang-format on
    {
      using category     = adaptative_neighborhood_tag;
      using incremental  = std::false_type;
      using decomposable = std::false_type;
      using separable    = std::false_type;

      ::ranges::subrange<P*> operator()(P p);

      ::ranges::subrange<Pix*> operator()(Pix px);
      ::ranges::subrange<P*>   offsets() const;
    };


    template <class SE>
    struct AsSE : SE, ano::details::StructuringElement<AsSE<SE>>
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

      bool                                                             is_decomposable() const;
      ::ranges::subrange<ano::archetypes::StructuringElement<P, Pix>*> decompose() const;
    };

    template <class P, class Pix>
    struct SeparableStructuringElement : StructuringElement<P, Pix>
    {
      using separable = std::true_type;

      bool                                                             is_separable() const;
      ::ranges::subrange<ano::archetypes::StructuringElement<P, Pix>*> separate() const;
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

} // namespace ano::archetypes
