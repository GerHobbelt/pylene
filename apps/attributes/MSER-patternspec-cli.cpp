#include <mln/accu/accumulators/max.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/image/image2d.hpp>

#include <mln/morpho/algebraic_filter.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/filtering.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>

#include "cMSER.hpp"
#include <apps/tos/topology.hpp>
#include <mln/accu/accumulators/accu_if.hpp>
#include <mln/accu/accumulators/count.hpp>

#include <mln/morpho/datastruct/checktree.hpp>
#include <mln/morpho/extinction.hpp>

int main(int argc, char** argv)
{
  if (argc < 9)
  {
    std::cerr
        << "Usage: " << argv[0] << "tree image.tiff λ ε t₁ t₂ spectra.csv\n"
        << " λ:\t Grain filter before anything else (number of 2F) (consider: 20-50, *20*) \n"
        << " ε:\t The delta level considered when fetching the neighborhood of a node in MSER (consider 5-20, *10*)\n"
        << " t₁:\t Threshold above which node having a MSER value greater that t₁ are removed (consider 0.7-2.0, "
           "*1.0*)\n"
        << " t₂:\t Threshold below which node having an extinction value lesser than t₂ are removed (consider 0-t₁, "
           "*0.2*).\n";
    std::exit(1);
  }

  using namespace mln;

  const char* tree_path = argv[1];
  const char* img_path = argv[2];
  unsigned grain = std::atoi(argv[3]);
  int eps = std::atoi(argv[4]);
  float threshold1 = std::atof(argv[5]);
  float threshold2 = std::atof(argv[6]);
  const char* out_path = argv[7];

  typedef morpho::component_tree<unsigned, image2d<unsigned>> tree_t;
  tree_t tree;
  {
    std::ifstream f(tree_path, std::ios::binary);
    morpho::load(f, tree);
  }

  image2d<uint16> ima;
  io::imread(img_path, ima);

  if (ima.domain() != tree._get_data()->m_pmap.domain())
  {
    std::cerr << "Domain between image differ.\n"
              << ima.domain() << " vs " << tree._get_data()->m_pmap.domain() << std::endl;
  }

  auto vmap = morpho::make_attribute_map_from_image(tree, ima);

  accu::accumulators::accu_if<accu::accumulators::count<>, K1::is_face_2_t, point2d> counter;
  auto amap = morpho::paccumulate(tree, ima, counter);

  auto pred = make_functional_property_map<tree_t::vertex_id_t>(
      [&amap, grain](tree_t::vertex_id_t x) { return amap[x] > grain; });
  morpho::filter_direct_inplace(tree, pred);
  morpho::internal::checktree(tree);

  auto amser = compute_MSER(tree, vmap, amap, eps, MSER_NORM);

  // convert to image and filter
  auto imser = morpho::make_image(tree, amser);
  {
    // float maxval = accumulate(imser, accu::features::max<> ());
    mln_foreach (float& v, imser.values())
      if (v > threshold1)
        v = threshold1;
  }

  auto nmser = morpho::extinction(imser, morpho::tree_neighb_t());
  // auto nmser = eval(nmser_ / imser);
  // auto nmser = morpho::area_closing(imser, morpho::tree_neighb_t(), areaAS);

  {
    std::ofstream fout(out_path);
    fout << "area,mser,extinction\n";
    mln_foreach (auto x, tree.nodes())
      fout << amap[x] << "," << amser[x] << "," << nmser(x) << "\n";
    fout.close();
  }

  {
    image2d<uint16> tmp;
    resize(tmp, ima);
    auto& attr = nmser.get_vmap();
    auto pred = [&attr, threshold2](const tree_t::node_type& x) { return threshold2 < attr[x]; };
    morpho::filter_direct_and_reconstruct(tree, make_functional_property_map<tree_t::node_type>(pred), vmap, tmp);
    io::imsave(tmp, "selection.tiff");
  }

  // image2d<float> out(200,200);
  // morpho::pattern_spectra(tree, amser, amap,
  //                         make_functional_property_map([&](const tree_t::node_type& x) -> float {
  //                             return amap[x];
  //                           }), out, true, true);
  // io::imsave(out, argv[4]);
}
