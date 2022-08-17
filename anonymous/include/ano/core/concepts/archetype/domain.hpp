#pragma once

#include <ano/core/concepts/archetype/point.hpp>
#include <ano/core/concepts/domain.hpp>


namespace ano::archetypes
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

  static_assert(ano::concepts::Domain<Domain>, "Domain archetype does not model the Domain concept!");

  struct SizedDomain : Domain
  {
    unsigned size() const;
  };

  static_assert(ano::concepts::SizedDomain<SizedDomain>,
                "SizedDomain archetype does not model the SizedDomain concept!");

  struct ShapedDomain final : SizedDomain
  {
    static constexpr std::size_t  ndim = 1;
    value_type                    shape() const;
    std::array<std::size_t, ndim> extents() const;
  };

  static_assert(ano::concepts::ShapedDomain<ShapedDomain>,
                "ShapedDomain archetype does not model the ShapedDomain concept!");
} // namespace ano::archetypes
