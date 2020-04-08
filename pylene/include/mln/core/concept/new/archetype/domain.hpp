#pragma once

#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/domains.hpp>


namespace mln::archetypes
{

  struct Domain
  {
    using value_type = Point;
    using reference  = Point&;

    value_type* begin();
    value_type* end();

    bool has(value_type) const;
    bool empty() const;
    int  dim() const;
  };

  static_assert(mln::concepts::Domain<Domain>, "Domain archetype does not model the Domain concept!");

  struct SizedDomain : Domain
  {
    unsigned size() const;
  };

  static_assert(mln::concepts::SizedDomain<SizedDomain>,
                           "SizedDomain archetype does not model the SizedDomain concept!");

  struct ShapedDomain final : SizedDomain
  {
    static constexpr std::size_t  ndim = 1;
    value_type                    shape() const;
    std::array<std::size_t, ndim> extents() const;
  };

  static_assert(mln::concepts::ShapedDomain<ShapedDomain>,
                           "ShapedDomain archetype does not model the ShapedDomain concept!");
} // namespace mln::archetypes
