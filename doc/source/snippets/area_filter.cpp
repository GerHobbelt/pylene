#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>


#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/area_filter.hpp>
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

  mln::image2d<bool> input;

  mln::io::imread(argv[1], input);

  // #M2_START
  // Make blobs connected
  auto disc = mln::se::disc(2);
  auto dil = mln::morpho::dilation(input, disc);

  // Filtering
  auto mask = mln::morpho::area_opening(dil, mln::c8, 2000);

  // Mask
  auto out = mask && input;
   // #M2_END

  // Save
  mln::io::imsave(dil, argv[2]);
  mln::io::imsave(mask, argv[3]);
  mln::io::imsave(out, argv[4]);
}
