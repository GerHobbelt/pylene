#pragma once

#include <mln/core/concepts/archetype/point.hpp>
#include <mln/core/concepts/archetype/value.hpp>
#include <mln/core/concepts/pixel.hpp>


namespace mln::archetypes
{

  namespace details
  {

    template <class P, class V>
    struct PixelT
    {
      using value_type = V;
      using point_type = P;
      using reference  = const value_type&;

      PixelT()              = delete;
      PixelT(const PixelT&) = default;
      PixelT(PixelT&&)      = default;
      PixelT& operator=(const PixelT&) = delete;
      PixelT& operator=(PixelT&&) = delete;

      point_type point() const;
      reference  val() const;
      void       shift(const P& dp);
    };

    struct OutputPixel : PixelT<Point, Value>
    {
      using reference = Value&;

      reference val() const;
    };


    template <class Pix>
    struct AsPixel : Pix, mln::details::Pixel<AsPixel<Pix>>
    {
    };
  } // namespace details

  template <class P, class V = Value>
  using PixelT      = details::AsPixel<details::PixelT<P, V>>;
  using Pixel       = PixelT<Point, Value>;
  using OutputPixel = details::AsPixel<details::OutputPixel>;

} // namespace mln::archetypes
