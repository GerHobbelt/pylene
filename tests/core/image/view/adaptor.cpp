#include <mln/core/concept/new/archetype/image.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/image/view/adaptor.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;


TEST(Core, ViewAdaptor_Pixel)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Pixel<mln::pixel_adaptor<archetypes::Pixel>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}

/*
#ifdef PYLENE_CONCEPT_TS_ENABLED
template <concepts::Image Ima>
void foo(Ima)
{
}
#endif // PYLENE_CONCEPT_TS_ENABLED
*/
TEST(Core, ViewAdaptor_Image)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED

  static_assert(concepts::Image<mln::image_adaptor<archetypes::Image>>);

  // TODO: all image properties + output Image
  // foo(mln::image_adaptor<archetypes::Image>{});

#endif // PYLENE_CONCEPT_TS_ENABLED
}
