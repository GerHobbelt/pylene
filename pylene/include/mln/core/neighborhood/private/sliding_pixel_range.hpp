#pragma once
#include <meta/meta.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/range_concepts.hpp>

namespace mln::details
{
  template <class Pixel, class Rng>
  struct sliding_pixel_range : ::ranges::view_adaptor<sliding_pixel_range<Pixel, Rng>, Rng>
  {
    static_assert(::ranges::View<Rng>());

  private:
    friend ::ranges::range_access;
    Pixel m_pix;

    struct adaptor : ::ranges::adaptor_base
    {
      Pixel m_pix;
      adaptor() = default;
      adaptor(Pixel px) : m_pix(std::move(px)) {}

      Pixel read(::ranges::iterator_t<Rng> it) const
      {
        Pixel tmp = m_pix;
        tmp.advance(*it);
        return tmp;
      }
    };

    adaptor begin_adaptor() const { return {m_pix}; }
    ::ranges::adaptor_base end_adaptor() const { return {}; }


  public:
    sliding_pixel_range(Pixel px, Rng rng)
      : sliding_pixel_range::view_adaptor { std::move(rng) },
        m_pix { std::move(px) }
      {
      }

    sliding_pixel_range() = default;

    CONCEPT_REQUIRES(::ranges::SizedRange<Rng const>())
    constexpr auto size() const { return ::ranges::size(this->base()); }
  };

}
