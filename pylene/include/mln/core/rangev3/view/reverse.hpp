#pragma once

#include <concepts/concepts.hpp>
#include <mln/core/rangev3/concepts.hpp>
#include <mln/core/rangev3/type_traits.hpp>
#include <mln/core/rangev3/private/mdrange_facade.hpp>

#include <range/v3/view/reverse.hpp>

namespace mln::ranges
{

  template <class Rng>
  requires MDBidirectionalRange<Rng>
  struct reversed_mdview : details::mdview_facade<reversed_mdview<Rng>>
  {
    using backward_cursor = ::ranges::detail::begin_cursor_t<Rng>;
    using cursor          = typename Rng::backward_cursor;

    backward_cursor              rbegin_cursor() const { return ::ranges::range_access::begin_cursor(m_rng); }
    ::ranges::default_sentinel_t rend_cursor() const { return {}; }

    cursor                       begin_cursor() const { return m_rng.rbegin_cursor(); }
    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    reversed_mdview() = default;
    reversed_mdview(Rng rng)
      : m_rng{std::move(rng)}
    {
    }

  private:
    Rng m_rng;
  };

  namespace view
  {
    template <class Rng>
    requires MDBidirectionalRange<Rng>
    auto reverse(Rng&& rng)
    {
      using R = ::concepts::remove_cvref_t<Rng>;
      return reversed_mdview<R>{std::forward<Rng>(rng)};
    }

    template <class R>
    requires ::ranges::cpp20::bidirectional_range<R>
    auto reverse(R&& rng)
    {
      return ::ranges::view::reverse(std::forward<R>(rng));
    }
  }

} // namespace mln::ranges::view
