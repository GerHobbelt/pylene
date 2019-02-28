#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>


namespace mln
{
  // clang-format off
  struct forward_image_tag
  {
  };
  struct bidirectional_image_tag : forward_image_tag
  {
  };
  struct[[deprecated]] random_access_image_tag : bidirectional_image_tag
  {
  };
  struct raw_image_tag : random_access_image_tag
  {
  };
  //clang-format on

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::stl::DerivedFrom<bidirectional_image_tag, forward_image_tag>);
  static_assert(concepts::stl::DerivedFrom<random_access_image_tag, bidirectional_image_tag>);
  static_assert(concepts::stl::DerivedFrom<random_access_image_tag, forward_image_tag>);
  static_assert(concepts::stl::DerivedFrom<raw_image_tag, random_access_image_tag>);
  static_assert(concepts::stl::DerivedFrom<raw_image_tag, bidirectional_image_tag>);
  static_assert(concepts::stl::DerivedFrom<raw_image_tag, forward_image_tag>);
#endif

} // namespace mln
