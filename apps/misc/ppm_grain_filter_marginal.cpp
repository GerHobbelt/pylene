#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/addborder.hpp>
#include <apps/tos/croutines.hpp>

#include <mln/accu/accumulators/accu_if.hpp>
#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/filtering.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>
#include <mln/morpho/tos/tos.hpp>


int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0]
              << " input(color) lambda output(gray)\n"
                 "Compute a grain filter marginally (on each components independenlty)\n";
    std::exit(1);
  }

  image2d<rgb8> f, F;
  image2d<rgb8> out;
  unsigned      lambda;

  io::imread(argv[1], f);
  lambda = std::atoi(argv[2]);

  box2d domain = f.domain();
  f            = addborder_marginal(f);
  F            = immerse_k1(f);
  resize(out, F);

  for (int i = 0; i < 3; ++i)
  {
    auto tree = morpho::tos(channel(f, i));
    grain_filter_inplace(tree, lambda);

    auto vmap = morpho::make_attribute_map_from_image(tree, channel(F, i));
    morpho::reconstruction(tree, vmap, channel(out, i));
  }

  out = Kadjust_to(out, domain);
  io::imsave(out, argv[3]);
}
