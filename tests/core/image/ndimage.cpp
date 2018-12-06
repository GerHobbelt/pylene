#include <mln/core/concept/new/images.hpp>

#include <mln/core/image/image2d.hpp>
#include <mln/core/image/ndimage.hpp>

#include <type_traits>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


#ifdef PYLENE_CONCEPT_TS_ENABLED
template <concepts::Image Ima>
void foo(Ima)
{
}
#endif // PYLENE_CONCEPT_TS_ENABLED

TEST(Core, Image_NdImage)
{

  using tested_image_type = mln::ndimage_base<int, 3, mln::image2d<int>>;

  [[maybe_unused]] auto img = tested_image_type{};
  foo(img);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // static_assert(concepts::Image<decltype(img)>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
