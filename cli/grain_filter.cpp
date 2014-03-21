#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/accu_if.hpp>
#include <mln/morpho/tos/ctos.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/filtering.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>

#include <apps/tos/addborder.hpp>
#include <apps/tos/topology.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include <iostream>


int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 4)
    {
      std::cerr << "Usage: " << argv[0] << " input(gray) lambda output(gray)" << std::endl;
      std::exit(1);
    }

  image2d<uint8> f;
  io::imread(argv[1], f);

  image2d<uint8> ima = addborder(f);
  image2d<uint8> Ima = immerse_k1(ima);

  typedef image2d<uint8>::size_type size_type;
  morpho::component_tree< size_type, image2d<size_type> > tree;
  tree = morpho::cToS(ima, c4);
  tree._reorder_pset();

  auto isface2 = [](const point2d& p) { return K1::is_face_2(p); };
  typedef accu::accumulators::accu_if< accu::accumulators::count<>, decltype(isface2), point2d> ACC;
  ACC accu(accu::accumulators::count<> (), isface2);

  auto areamap = morpho::paccumulate(tree, Ima, accu, false);

  unsigned lambda = std::atoi(argv[2]);
  auto criterion = make_functional_property_map
    ([&areamap, lambda](size_type x) {
      return areamap[x] >= lambda;
    });

  morpho::filter_min_inplace(tree, criterion);

  auto valuemap = morpho::make_attribute_map_from_image(tree, Ima);

  // {
  //   mln_foreach(auto x, tree.nodes())
  //     std::cout << "---" << x.id() << "\tparent: "<< x.parent().id()
  // 		<< "\tval  : " << (int)valuemap[x.id()]
  // 		<< "\tprev : " << x.get_prev_node_id()
  // 		<< "\tnext : " << x.get_next_node_id()
  // 		<< "\tnexts: " << x.get_next_sibling_id() <<  std::endl;
  // }


  image2d<uint8> out;
  resize(out, tree._get_data()->m_pmap);
  morpho::reconstruction(tree, valuemap, out);

  image2d<uint8> res;
  resize(res, f);
  copy(out | sbox2d{out.domain().pmin + point2d{2,2},
	            out.domain().pmax - point2d{2,2},
		      point2d{2,2}}, res);

  io::imsave(res, argv[3]);
}
