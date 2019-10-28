#pragma once

#include <mln/core/private/weighted_pixel.hpp>

#include <meta/meta.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/range/concepts.hpp>

namespace mln::details
{
  template <class Pixel, class Rng>
  struct sliding_pixel_range : ::ranges::view_adaptor<sliding_pixel_range<Pixel, Rng>, Rng>
  {
    static_assert(::ranges::cpp20::view<Rng>);

  private:
    friend ::ranges::range_access;
    ::ranges::semiregular_t<Pixel> m_pix;

    struct adaptor : ::ranges::adaptor_base
    {
      ::ranges::semiregular_t<Pixel> m_pix;
      adaptor() = default;
      adaptor(Pixel px)
        : m_pix(std::move(px))
      {
      }

      weighted_pixel<Pixel> read(::ranges::iterator_t<Rng> it) const
      {
        Pixel tmp = m_pix;
        tmp.shift(*it);
        return {tmp};
      }
    };

    adaptor                begin_adaptor() const { return {m_pix}; }
    ::ranges::adaptor_base end_adaptor() const { return {}; }


  public:
    sliding_pixel_range(Pixel px, Rng rng)
      : sliding_pixel_range::view_adaptor{std::move(rng)}
      , m_pix{std::move(px)}
    {
    }

    sliding_pixel_range() = default;


    constexpr auto size() const requires(::ranges::cpp20::sized_range<Rng const>)
    {
      return ::ranges::size(this->base());
    }
  };

  template <class Pixel, class Rng>
  struct sliding_wpixel_range : ::ranges::view_adaptor<sliding_wpixel_range<Pixel, Rng>, Rng>
  {
    static_assert(::ranges::cpp20::view<Rng>);

  private:
    friend ::ranges::range_access;
    ::ranges::semiregular_t<Pixel> m_pix;

    struct adaptor : ::ranges::adaptor_base
    {
      ::ranges::semiregular_t<Pixel> m_pix;
      adaptor() = default;
      adaptor(Pixel px)
        : m_pix(std::move(px))
      {
      }

      auto read(::ranges::iterator_t<Rng> it) const -> mln::weighted_pixel<Pixel, decltype(it->weight())>
      {
        Pixel tmp = m_pix;
        tmp.shift(it->point());
        return {tmp, it->weight()};
      }
    };

    adaptor                begin_adaptor() const { return {m_pix}; }
    ::ranges::adaptor_base end_adaptor() const { return {}; }


  public:
    sliding_wpixel_range(Pixel px, Rng rng)
      : sliding_wpixel_range::view_adaptor{std::move(rng)}
      , m_pix{std::move(px)}
    {
    }

    sliding_wpixel_range() = default;


    constexpr auto size() const requires(::ranges::cpp20::sized_range<Rng const>)
    {
      return ::ranges::size(this->base());
    }
  };
}
