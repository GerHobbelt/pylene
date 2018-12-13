#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/image/view/adaptor.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;

#ifdef PYLENE_CONCEPT_TS_ENABLED
template <concepts::Pixel Pix>
void foo(Pix)
{
}
#endif // PYLENE_CONCEPT_TS_ENABLED

TEST(Core, ViewAdaptor_Pixel)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED

  // foo(mln::pixel_adaptor<archetypes::Pixel>{});
  // static_assert(concepts::Pixel<mln::pixel_adaptor<archetypes::Pixel>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
