#ifndef MLN_INPUT_VALUE_TYPE
# define MLN_INPUT_VALUE_TYPE mln::uint16
#endif

#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/colors/rgba.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/algorithm/transform.hpp>

#include <apps/tos/Kinterpolate.hpp>
#include <apps/saliency/attribute_on_contour.hpp>

#include <mln/morpho/component_tree/io.hpp>
//#include <mln/morpho/component_tree/graphviz.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>
#include <mln/morpho/component_tree/filtering.hpp>
#include <mln/morpho/extinction.hpp>

#include <mln/accu/accumulators/accu_if.hpp>
#include <mln/accu/accumulators/count.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include "cMeaningFullNess.hpp"
#include <fstream>

int main(int argc, char** argv)
{
  if (argc < 4)
    {
      std::cerr << "Usage:" << argv[0] << "tree input.tiff λ ε α β γ out.tree spectra.csv\n"
                << "The tree and input must hold the 0 and 1 faces.\n"
                << " λ:\t Grain filter before anything else (number of 2F)\n"
                << " ε:\t The radius of the ball when considering the contextual energy (3-10).\n"
                << " α, β, γ:\tParameters for energy\n"
                << "\tE(Γ) = α V₁(Γ) + βV₂(Γ) + V₃(Γ)\n"
                << "\twith V₁(Γ) = (ExternalVar(Γ,ε) + InternalVar(Γ,ε)) / Var(Γ,ε)\n"
                << "\t     V₂(Γ) = Mean Curvature(∂Γ)\n"
                << "\t     V₂(Γ) = exp(-γ |∂Γ|/|∂Ω|)\n"
                << "\t     Consider α=1, β=1, γ=10-100\n"
                << "\t     (you can set one of these parameters to 0 to unactive the term).\n"
                << " t₁:\t Threshold above which node having an energy greater that t₁ are removed.\n";
      // << " t₂:\t Threshold below which node having an extinction value lesser than t₂ are removed.\n";
      std::abort();
    }

  using namespace mln;


  const char* tree_path = argv[1];
  const char* img_path = argv[2];
  unsigned grain = std::atoi(argv[3]);
  int eps = std::atoi(argv[4]);
  float alpha = std::atof(argv[5]);
  float beta = std::atof(argv[6]);
  float gamma = std::atof(argv[7]);
  float threshold1 = std::atof(argv[8]);
  const char* out_tree = argv[9];
  const char* output_path = argv[10];

  typedef morpho::component_tree<unsigned, image2d<unsigned> > tree_t;
  tree_t tree;
  {
    std::ifstream f(tree_path, std::ios::binary);
    morpho::load(f, tree);
  }

  typedef MLN_INPUT_VALUE_TYPE V;

  image2d<V> ima_, ima;
  io::imread(img_path, ima_);

  ima = Kadjust_to(ima_, tree._get_data()->m_pmap.domain());

  if (ima.domain() != tree._get_data()->m_pmap.domain())
    {
      std::cerr << "Domain between image differ.\n"
                << ima.domain() << " vs "
                << tree._get_data()->m_pmap.domain() << std::endl;
    }


  image2d<V> F = ima;
  image2d<V> f = unimmerse_k1(F);
  image2d<float> C = compute_curvature(transform(f, [](V x) { return l1norm(x); }));

  accu::accumulators::accu_if< accu::accumulators::count<>,
                               K1::is_face_2_t,
                               point2d > counter;
  auto areamap = morpho::paccumulate(tree, F, counter);

  auto pred = make_functional_property_map<tree_t::vertex_id_t>([&areamap, grain](tree_t::vertex_id_t x) {
      return areamap[x] > grain;
    });
  morpho::filter_direct_inplace(tree, pred);
  {
    std::ofstream f(out_tree);
    morpho::save(tree, f);
  }
  typedef std::conditional< std::is_scalar<V>::value, double,
                            vec<double, value_traits<V>::ndim> >::type SumType;
  auto amap = compute_regional_energy(tree, F,
                                     accu::features::variance<SumType> (),
                                     eps);
  amap[tree.get_root()] = 0;

  auto amap2 = compute_attribute_on_contour(tree, C,
                                            accu::features::count<unsigned> () &
                                            accu::features::mean<double> ());

  // Compute energy and filter
  property_map<tree_t, float> energy(tree);
  {
    point2d shp = ima.domain().shape();
    float domlength = 2*shp[0] + 2*shp[1] - 4;
    mln_foreach(auto x, tree.nodes())
      {
        float v1 = amap[x][2] > 0 ? (alpha * (amap[x][0]+amap[x][1])/amap[x][2]) : 0;
        float v2 = beta * accu::extractor::mean(amap2[x]);
        float v3 = std::exp(-gamma * accu::extractor::count(amap2[x]) / domlength);
        energy[x] = v1+v2+v3;
      }
  }

   // convert to image and filter
  auto ienergy = morpho::make_image(tree, energy);
  {
    mln_foreach(float& v, ienergy.values())
      if (v > threshold1)
        v = threshold1;
  }

  auto extincted = morpho::extinction(ienergy, morpho::tree_neighb_t());

  // {
  //   std::ofstream f("out.dot");
  //   morpho::write_graphviz(f, tree,
  //                          make_functional_property_map<tree_t::node_type> ([&amap2](const tree_t::node_type& n) {
  //                              return accu::extractor::mean(amap2[n]);
  //                            })
  //                          );
  // }

  // Output attributes
  {
    std::ofstream f(output_path);
    f << "Area,Vin,Vout,Vtotal,Var,Cont. Length,Curv. Avg.,Energy,Extinction\n";
    mln_foreach(auto x, tree.nodes())
      f << areamap[x] << ","
        << amap[x][0] << ","
        << amap[x][1] << ","
        << amap[x][2] << ","
        << (amap[x][0]+amap[x][1])/amap[x][2] << ","
        << accu::extractor::count(amap2[x]) << ","
        << accu::extractor::mean(amap2[x]) << ","
        << energy[x] << ","
        << extincted(x) << "\n";
  }
  /*
  {
    image2d<uint16> tmp;
    resize(tmp, F);
    auto& attr = extincted.get_vmap();
    auto pred = [&attr, threshold2](const tree_t::node_type& x) { return threshold2 < attr[x]; };

    auto vmap = morpho::make_attribute_map_from_image(tree, F);
    morpho::filter_direct_and_reconstruct(tree, make_functional_property_map<tree_t::node_type>(pred),
                                          vmap, tmp);
    io::imsave(tmp, "selection.tiff");

    image2d<float> sal = imchvalue<float>(F).init(0);
    attribute_on_contour(tree, attr, sal);
    io::imsave(sal, "saliency.tiff");
  }
  */
}
