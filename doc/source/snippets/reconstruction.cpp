#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>


#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>
#include <mln/morpho/rank_filter.hpp>
#include <mln/morpho/reconstruction.hpp>
#include <mln/core/image/view/operators.hpp>
#include <ratio>

int main(int argc, char** argv)
{
  if (argc < 6)
  {
    std::cerr << "Usage: " << argv[0] << " dilated.pbm markers.pbm reconstruction.pbm out.pbm\n";
    return 1;
  }

  using namespace mln::view::ops;

  mln::image2d<bool> input;

  mln::io::imread(argv[1], input);

  // #M2_START
  // Make blobs connected
  auto disc = mln::se::disc(4);
  auto dil = mln::morpho::dilation(input, disc);

  // Get markers for large connected components
  auto rect = mln::se::rect2d(20, 20);
  auto markers = mln::morpho::rank_filter<std::ratio<1,4>>(input, rect, mln::extension::bm::fill(false));

  // Reconstruction of the large CC
  auto rec = mln::morpho::opening_by_reconstruction(dil, markers, mln::c8);
   // #M2_END

  // Mask
  auto out = mln::clone(rec && input);

  // Save

  mln::io::imsave(dil, argv[2]);
  mln::io::imsave(markers, argv[3]);
  mln::io::imsave(rec, argv[4]);
  mln::io::imsave(out, argv[5]);
}
