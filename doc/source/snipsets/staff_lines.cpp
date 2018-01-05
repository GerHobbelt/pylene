#include <mln/morpho/hit_or_miss.hpp>
#include <mln/morpho/opening_by_reconstruction.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/se/utility.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>



int main(int argc, char** argv)
{
  if (argc < 7)
  {
    std::cerr << "Usage: " << argv[0] << " markers1.pbm markers2.pbm markers.pbm all.pbm lines.pbm\n";
    return 1;
  }

  mln::image2d<bool> input;
  mln::io::imread(argv[1], input);

  // Make a dual neighborhood corresponding to
  // x x x
  // o o o
  // x x x

  mln::image2d<bool> markers1, markers2;
  {
    std::array<mln::point2d, 3> se_hit_data = { { {0, -1}, {0, 0}, {0, 1} } };
    std::array<mln::point2d, 6> se_miss_data = { {
        {-1, -1}, {-1, 0}, {-1, 1},
        {+1, -1}, {+1, 0}, {+1, 1}
      } };
    auto se_hit = mln::se::make_se(se_hit_data);
    auto se_miss = mln::se::make_se(se_miss_data);
    auto output = mln::morpho::hit_or_miss(input, se_hit, se_miss);
    markers1 = output;
    mln::io::imsave(mln::lnot(markers1), argv[2]);
  }

  {
    std::array<mln::point2d, 6> se_hit_data = { {
        {0, -1}, {0, 0}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
      } };
    std::array<mln::point2d, 6> se_miss_data = { {
        {-1, -1}, {-1, 0}, {-1, 1},
        {+2, -1}, {+2, 0}, {+2, 1}
      } };
    auto se_hit = mln::se::make_se(se_hit_data);
    auto se_miss = mln::se::make_se(se_miss_data);
    auto output = mln::morpho::hit_or_miss(input, se_hit, se_miss);
    markers2 = output;
    mln::io::imsave(mln::lnot(markers2), argv[3]);
  }

  auto markers = mln::lor(markers1, markers2);
  mln::io::imsave(mln::lnot(markers), argv[4]);

  auto all_touching = mln::morpho::opening_by_reconstruction(input, markers, mln::c4);
  mln::io::imsave(mln::lnot(all_touching), argv[5]);

  auto lines_only = mln::morpho::opening_by_reconstruction(input, markers, mln::c2_h);
  mln::io::imsave(mln::lnot(lines_only), argv[6]);
}
