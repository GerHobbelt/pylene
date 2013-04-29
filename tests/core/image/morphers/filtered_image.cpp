#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/image/morphers/filtered_image.hpp>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/fill.hpp>

#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>
#include <mln/io/imprint.hpp>



BOOST_AUTO_TEST_SUITE(FilteredImage)


BOOST_AUTO_TEST_CASE(filtered_bypix)
{
  using namespace mln;

  typedef image2d<int> I;

  box2d dom{{-1,-2}, {3,3}};
  image2d<int> ima(dom);

  typedef I::const_pixel_type Pix;

  iota(ima, 0);
  auto x = imfilter(ima, [](const Pix& px) { return px.val() > 10; });

  BOOST_CHECK(all(x > 10));

  {
    mln_foreach(const Pix& pix, ima.pixels())
      if (pix.val() > 10)
        BOOST_CHECK_EQUAL(pix.val(), x(pix.point()));
      else {
        BOOST_CHECK(!x.domain().has(pix.point()));
        BOOST_CHECK_EQUAL(pix.val(), x.at(pix.point()));
      }
  }

  {
    mln_foreach(const auto& pix, x.pixels()) {
      BOOST_CHECK_EQUAL(pix.val(), ima(pix.point()));
    }
  }

}


BOOST_AUTO_TEST_CASE(filtered_image_writing)
{
  using namespace mln;

  typedef image2d<int> I;

  box2d dom{{-1,-2}, {3,3}};
  image2d<int> ima(dom);

  typedef I::const_pixel_type Pix;

  iota(ima, 0);
  auto x = imfilter(ima, [](const Pix& px) { return px.val() > 10; });

  fill(x, 10);

  io::imprint(ima);

}

BOOST_AUTO_TEST_SUITE_END()
