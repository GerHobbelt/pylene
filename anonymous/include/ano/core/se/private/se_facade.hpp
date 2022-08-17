#pragma once

#include <ano/core/concepts/object.hpp>
#include <ano/core/concepts/pixel.hpp>
#include <ano/core/concepts/structuring_element.hpp>

#include <ano/core/neighborhood/private/neighborhood_facade.hpp>
#include <ano/core/neighborhood/private/sliding_pixel_range.hpp>
#include <range/v3/view/transform.hpp>

#include <type_traits>


namespace ano
{
  template <class SE>
  struct se_facade : ano::details::StructuringElement<SE>
  {
    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const SE*>(this)->offsets()};
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const SE*>(this)->offsets(), details::add_point<P>{point});
    }
  };

} // namespace ano
