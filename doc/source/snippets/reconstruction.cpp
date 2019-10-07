#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>


#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/experimental/rank_filter.hpp>
#include <mln/morpho/experimental/reconstruction.hpp>
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

  mln::experimental::image2d<bool> input;

  mln::io::experimental::imread(argv[1], input);

  // #M2_START
  // Make blobs connected
  auto disc = mln::experimental::se::disc(4);
  auto dil = mln::morpho::experimental::dilation(input, disc);

  // Get markers for large connected components
  auto rect = mln::experimental::se::rect2d(20, 20);
  auto markers = mln::morpho::experimental::rank_filter<std::ratio<1,4>>(input, rect, mln::extension::bm::fill(false));

  // Reconstruction of the large CC
  auto rec = mln::morpho::experimental::opening_by_reconstruction(dil, markers, mln::experimental::c8);
   // #M2_END

  // Mask
  auto out = mln::clone(rec && input);

  // Save

  mln::io::experimental::imsave(dil, argv[2]);
  mln::io::experimental::imsave(markers, argv[3]);
  mln::io::experimental::imsave(rec, argv[4]);
  mln::io::experimental::imsave(out, argv[5]);
}
