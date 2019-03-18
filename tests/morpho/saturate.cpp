#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/saturate.hpp>

#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>


// FIXME:
namespace to_migrate
{
  template <typename I>
  inline bool __all(const mln::Image<I>& ima)
  {
    static_assert(std::is_convertible<typename I::value_type, bool>::value,
                  "Image value type must be convertible to bool");

    mln_viter(v, exact(ima));
    mln_forall (v)
      if (!*v)
        return false;

    return true;
  }
} // namespace to_migrate

TEST(Morpho, saturate)
{

  using namespace mln;

  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("squares.pgm"), ima);

  image2d<bool> out = morpho::saturate(ima == 154, c4, point2d{0, 0});
  ASSERT_TRUE(::to_migrate::__all(out == lor(lor(ima == 154, ima == 251), ima == 12)));

  morpho::saturate(ima == 130, c4, point2d{0, 0}, out);
  ASSERT_TRUE(::to_migrate::__all(out == (ima >= 12)));

  // pinf \in A => sat(A) == domain(f)
  morpho::saturate(ima == 195, c4, point2d{69, 45}, out);
  ASSERT_TRUE(::to_migrate::__all(out == true));

  // io::imprint(out);
}
