#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>


#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/area_filter.hpp>
#include <mln/core/image/view/operators.hpp>
#include <ratio>

int main(int argc, char** argv)
{
  if (argc < 5)
  {
    std::cerr << "Usage: " << argv[0] << " input dilated mask output\n";
    return 1;
  }

  using namespace mln::view::ops;

  mln::experimental::image2d<bool> input;

  mln::io::experimental::imread(argv[1], input);

  // #M2_START
  // Make blobs connected
  auto disc = mln::experimental::se::disc(2);
  auto dil = mln::morpho::experimental::dilation(input, disc);

  // Filtering
  auto mask = mln::morpho::experimental::area_opening(dil, mln::experimental::c8, 2000);

  // Mask
  auto out = mask && input;
   // #M2_END

  // Save
  mln::io::experimental::imsave(dil, argv[2]);
  mln::io::experimental::imsave(mask, argv[3]);
  mln::io::experimental::imsave(out, argv[4]);
}
