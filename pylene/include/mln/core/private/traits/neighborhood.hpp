#pragma once

#include <type_traits>


namespace mln
{

  struct adaptative_neighborhood_tag
  {
  };
  struct dynamic_neighborhood_tag : adaptative_neighborhood_tag
  {
  };
  struct static_neighborhood_tag : dynamic_neighborhood_tag
  {
  };
  struct constant_neighborhood_tag : static_neighborhood_tag
  {
  };

  template <typename N>
  struct neighborhood_traits
  {
    using category       = typename N::category;
    using is_incremental = typename N::is_incremental;
  };

  // Some traits helper
  template <class N>
  struct neighborhood_is_constant : std::is_convertible<typename N::category, dynamic_neighborhood_tag>
  {
  };
} // namespace mln
