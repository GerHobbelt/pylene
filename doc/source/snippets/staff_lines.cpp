#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c2_h.hpp>
#include <mln/core/se/mask2d.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>
#include <mln/morpho/experimental/hit_or_miss.hpp>
#include <mln/morpho/experimental/reconstruction.hpp>


int main(int argc, char** argv)
{
  using namespace mln::view::ops;

  if (argc < 7)
  {
    std::cerr << "Usage: " << argv[0] << " markers1.pbm markers2.pbm markers.pbm all.pbm lines.pbm\n";
    return 1;
  }

  mln::experimental::image2d<bool> input;
  mln::io::experimental::imread(argv[1], input);

  // Make a dual neighborhood corresponding to
  // x x x
  // o o o
  // x x x

  mln::experimental::image2d<bool> markers1, markers2;
  { // #M1_START
    mln::se::experimental::mask2d se_hit  = {{0, 0, 0}, {1, 1, 1}, {0, 0, 0}};
    mln::se::experimental::mask2d se_miss = {{1, 1, 1}, {0, 0, 0}, {1, 1, 1}};

    markers1 = mln::morpho::experimental::hit_or_miss(input, se_hit, se_miss);
    mln::io::experimental::imsave(not markers1, argv[2]);
  } // #M1_END

  { // #M2_START
    mln::se::experimental::mask2d se_hit  = {{0, 0, 0}, {0, 0, 0}, {1, 1, 1}, {1, 1, 1}, {0, 0, 0}};
    mln::se::experimental::mask2d se_miss = {{0, 0, 0}, {1, 1, 1}, {0, 0, 0}, {0, 0, 0}, {1, 1, 1}};

    markers2 = mln::morpho::experimental::hit_or_miss(input, se_hit, se_miss);
    mln::io::experimental::imsave(not markers2, argv[3]);
  } // #M2_END

  auto markers = markers1 or markers2;

  mln::io::experimental::imsave(not markers, argv[4]);

  // #M3_START
  auto markers_     = markers1 or markers2;
  auto all_touching = mln::morpho::experimental::opening_by_reconstruction(input, markers_, mln::experimental::c4);
  mln::io::experimental::imsave(not all_touching, argv[5]);
  // #M3_END

  // #M4_START
  auto lines_only = mln::morpho::experimental::opening_by_reconstruction(input, markers_, mln::experimental::c2_h);
  mln::io::experimental::imsave(not lines_only, argv[6]);
  // #M4_END
}
