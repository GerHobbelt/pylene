#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/accu/accumulators/variance.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/filtering.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>
#include <mln/morpho/tos/ctos.hpp>

int
main()
{
  using mln::uint8;

  mln::image2d<uint8> f;
  mln::io::imread("images/roadsigns.png", f);

  // Compute the ToS
  auto t = mln::morpho::cToS(f, mln::c4);
  typedef decltype(t) tree_t;

  // Set f to the right size
  mln::image2d<uint8> f2;
  {
    mln::resize(f2, t._get_data()->m_pmap);
    mln::box2d d = f2.domain();
    mln::copy(f, f2 | mln::sbox2d{d.pmin, d.pmax, {2, 2}});

    for (unsigned i = 0; i < f2.nrows(); i += 2)
      for (unsigned j = 0; j < f2.ncols(); j += 2)
      {
        f2.at(i, j + 1) = f2.at(i, j);
        f2.at(i + 1, j) = f2.at(i, j);
        f2.at(i + 1, j + 1) = f2.at(i, j);
      }
  }

  // Compute the bounding box & the size of the component
  auto prop_map = mln::morpho::paccumulate(
      t, f2, mln::accu::features::inf<>() & mln::accu::features::sup<>() & mln::accu::features::count<>());

  auto var_map = mln::morpho::vaccumulate(t, f2, mln::accu::features::variance<>());

  // Predicate: keep the node if it fits 95% of the bbox
  // and its size is less than 100000px
  auto pred = [&prop_map, &var_map](tree_t::vertex_id_t x) {
    mln::point2d pmin = mln::accu::extractor::inf(prop_map[x]);
    mln::point2d pmax = mln::accu::extractor::sup(prop_map[x]);
    unsigned sz = mln::accu::extractor::count(prop_map[x]);
    unsigned area_bbox = (pmax[0] - pmin[0] + 1) * (pmax[1] - pmin[1] + 1);
    double var = var_map[x];
    return sz > (0.95 * area_bbox) and sz > 10000 and sz < 1000000 and var > 250;
  };

  auto pred_ = mln::make_functional_property_map<tree_t::vertex_id_t>(pred);

  // Filter
  mln::morpho::filter_direct_inplace(t, pred_);

  // Reconstruct
  mln::image2d<uint8> out;
  mln::resize(out, t._get_data()->m_pmap);

  auto vmap = mln::morpho::make_attribute_map_from_image(t, f2);
  mln::morpho::reconstruction(t, vmap, out);
  mln::io::imsave(out, "images/roadsigns-square-2.png");
}
