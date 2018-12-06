#pragma once

#include <mln/core/concept/new/archetype/domain.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/images.hpp>

#include <type_traits>


namespace mln::concepts::archetype
{
  struct image_archetype;

  struct image_archetype final
  {
    using new_pixel_type = pixel_archetype;
    using value_type     = pixel_archetype::value_type;
    using reference      = pixel_archetype::reference;
    using point_type     = domain_archetype::value_type;
    using domain_type    = domain_archetype;
    using category_type  = new_forward_image_tag;
    using concrete_type  = image_archetype;

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <Value Val>
#else
    template <typename Val>
#endif // PYLENE_CONCEPT_TS_ENABLED
    using ch_value_type = image_archetype;

    // additional traits
    using extension_category = mln::extension::none_extension_tag;
    using indexable          = std::false_type;
    using accessible         = std::false_type;
    using bidirectional      = std::false_type;
    using raw                = std::false_type;


#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <Value Val>
#else
    template <typename Val>
#endif // PYLENE_CONCEPT_TS_ENABLED
    ch_value_type<Val> ch_value() const;
    concrete_type      concretize() const;
    domain_type        domain() const;


    struct pixel_range_archetype final
    {
      using value_type = new_pixel_type;
      using reference  = new_pixel_type&;
      new_pixel_type* begin();
      new_pixel_type* end();
    };

    pixel_range_archetype new_pixels();


    struct value_range_archetype final
    {
      using value_type = image_archetype::value_type;
      using reference  = image_archetype::reference;
      image_archetype::value_type* begin();
      image_archetype::value_type* end();
    };

    value_range_archetype new_values();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(Image<image_archetype>, "image_archetype does not model the Image concept!");
#endif // #ifdef PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::concepts::archetype