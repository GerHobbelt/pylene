#include "profiles.hpp"

#include <apps/tos/croutines.hpp>

#include <mln/morpho/tos/tos.hpp>

#include <boost/format.hpp>
#include <tbb/parallel_for.h>

#include <iostream>


namespace mln
{

  std::vector<image2d<float>> run(const image2d<uint16>& f, std::vector<image2d<uint16>>& rec)
  {
    tree_t tree = morpho::tos(f);
    grain_filter_inplace(tree, 10);

    image2d<uint16> F = Kadjust_to(f, tree._get_data()->m_pmap.domain());

    auto                        vmap = morpho::make_attribute_map_from_image(tree, F);
    std::vector<float>          lambdas;
    std::vector<image2d<float>> profiles = profile(tree, F, vmap, lambdas, rec);
    return profiles;
  }


} // namespace mln


int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " input.tiff stem\n";
    std::exit(1);
  }

  image2d<rgb16> f;
  io::imread(argv[1], f);

  f = addborder_marginal(f);

  std::vector<image2d<float>>  profiles[3];
  std::vector<image2d<uint16>> rec[3];

  tbb::parallel_for(0, 3, [&rec, &f, &profiles](int i) { profiles[i] = run(eval(channel(f, i)), rec[i]); });

  std::string    stem   = argv[2];
  int            n      = std::min(rec[0].size(), std::min(rec[1].size(), rec[2].size()));
  float          lambda = 0;
  image2d<rgb16> r;
  resize(r, rec[0][0]);
  for (int i = 0; i < n; ++i, lambda += 0.1)
  {
    mln::copy(rec[0][i], red(r));
    mln::copy(rec[1][i], green(r));
    mln::copy(rec[2][i], blue(r));
    io::imsave(r, (boost::format("%s-rec-%.02f.tiff") % stem % lambda).str());
  }
}
