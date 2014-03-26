#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>

#include <mln/morpho/tos/ctos.hpp>
#include <mln/morpho/component_tree/graphviz.hpp>
#include <mln/accu/accumulators/accu_if.hpp>
#include <mln/accu/accumulators/count.hpp>

#include <boost/format.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>

#include <apps/tos/addborder.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/topology.hpp>
#include <apps/g2/g2.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <fstream>

#include <boost/graph/random_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/graph/transpose_graph.hpp>
//#include <boost/random/mersenne_twister.hpp>

#include "remove_parent_relation.hpp"
#include "reconstruct.hpp"

namespace mln
{

  template <typename Graph>
  void write_graphviz(const std::string& filename,
		      const image2d<rgb8>& f,
		      const tree_t& t1, const tree_t& t2, const tree_t& t3,
		      const property_map<tree_t, typename Graph::vertex_descriptor> t1link,
		      const property_map<tree_t, typename Graph::vertex_descriptor> t2link,
		      const property_map<tree_t, typename Graph::vertex_descriptor> t3link,
		      const Graph& graph)
  {
    /// Write the three trees
    {
      auto val = make_attribute_map_from_image(t1, immerse_k1(red(f)));
      auto prop = [&](const tree_t::node_type& x) {
	return (boost::format("%i\\nval:%i") % t1link[x] % (int)val[x]).str();
      };
      std::ofstream outs(filename + "-t1.dot");
      morpho::write_graphviz(outs, t1, make_functional_property_map(prop));
      outs.close();
    }
    {
      auto val = make_attribute_map_from_image(t2, immerse_k1(green(f)));
      auto prop = [&](const tree_t::node_type& x) {
	return (boost::format("%i\\nval:%i") % t2link[x] % (int)val[x]).str();
      };
      std::ofstream outs(filename + "-t2.dot");
      morpho::write_graphviz(outs, t2, make_functional_property_map(prop));
      outs.close();
    }
    {
      auto val = make_attribute_map_from_image(t3, immerse_k1(blue(f)));
      auto prop = [&](const tree_t::node_type& x) {
	return (boost::format("%i\\nval:%i") % t3link[x] % (int)val[x]).str();
      };
      std::ofstream outs(filename + "-t3.dot");
      morpho::write_graphviz(outs, t3, make_functional_property_map(prop));
      outs.close();
    }
    /// Write the graph
    {
      std::ofstream outs(filename + ".dot");
      boost::write_graphviz(outs, graph);
      outs.close();
    }
  }
}

/// For an edge 
template <class WeightPropertyMap>
void
make_minimum_spanning_tree(Graph& g, const WeightPropertyMap& weights)
{
  mln_entering("Minimum Spanning tree computation");
  Graph tg;
  boost::transpose_graph(g, tg);

  auto idxpmap = boost::get(boost::vertex_index, tg);
  boost::vector_property_map<Graph::vertex_descriptor, decltype(idxpmap)> parent(boost::num_vertices(tg), idxpmap);

  boost::prim_minimum_spanning_tree(tg, parent,
				    (boost::root_vertex(boost::vertex(0, tg)),
				     boost::weight_map(weights)));

  std::cout << "Before: num edges " << boost::num_edges(g) << std::endl;
  // Update g2
  boost::remove_edge_if([&tg, &parent](const Graph::edge_descriptor& e) {
      return parent[boost::source(e, tg)] != boost::target(e, tg); },
    g);

  std::cout << "aFTER: num edges " << boost::num_edges(g) << std::endl;
  mln_exiting();
}


int main(int argc, char** argv)
{
  if (argc < 3)
    {
      std::cerr << "Usage: " << argv[0] << " input.ppm grain"
		<< std::endl;
      std::exit(1);
    }

  using namespace mln;

  image2d<rgb8> ima;
  io::imread(argv[1], ima);


  image2d<rgb8> f = addborder(ima, lexicographicalorder_less<rgb8>());
  image2d<rgb8> F = immerse_k1(f);

  typedef uint8 V;
  image2d<V> r = transform(f, [](rgb8 x) -> V { return x[0]; });
  image2d<V> g = transform(f, [](rgb8 x) -> V { return x[1]; });
  image2d<V> b = transform(f, [](rgb8 x) -> V { return x[2]; });


  /// Compute the marginal ToS
  auto t1 = morpho::cToS(red(f), c4);
  auto t2 = morpho::cToS(green(f), c4);
  auto t3 = morpho::cToS(blue(f), c4);

  /// Compute the graph
  Graph g2;
  property_map<tree_t, Graph::vertex_descriptor> t1link, t2link, t3link;
  std::tie(g2, t1link, t2link, t3link) = compute_g2(t1, t2, t3);

  auto glink = boost::get(&graph_content::tlinks, g2);


  /// Make a tree from the graph
  {
    // Put the weight on the edges: red edge -> 0 / GB edge -> 1 (thus we keep the first tree intact)
    auto isrededge = [&g2, &glink, &t2](const Graph::edge_descriptor& e) {
      std::cout << e << std::endl;
      return (int)(glink[boost::target(e, g2)][1] == t2.nend());
    };
    boost::function_property_map<decltype(isrededge), Graph::edge_descriptor, int>  weights(isrededge);

    //write_graphviz("before", f,  t1, t2, t3, t1link, t2link, t3link, g2);
    make_minimum_spanning_tree(g2, weights);
    update_parent_relation(g2, t1, t2, t3, t1link, t2link, t3link);
    //write_graphviz("after", f,  t1, t2, t3, t1link, t2link, t3link, g2);
  }

  // Filter graph
  accu::accumulators::accu_if<accu::accumulators::count<>, K1::is_face_2_t, point2d> acc;
  auto a1 = morpho::paccumulate(t1, F, acc);
  auto a2 = morpho::paccumulate(t2, F, acc);
  auto a3 = morpho::paccumulate(t3, F, acc);

  // 4. reconstruction
  auto val1 = make_attribute_map_from_image(t1, immerse_k1(r));
  auto val2 = make_attribute_map_from_image(t2, immerse_k1(g));
  auto val3 = make_attribute_map_from_image(t3, immerse_k1(b));

  t1._reorder_pset();
  t2._reorder_pset();
  t3._reorder_pset();


  image2d<rgb8> out, tmp;
  out.resize(box2d{f.domain().pmin, f.domain().pmax * 2 - 1});
  resize(tmp, ima);

  for (int i = 2; i < argc; ++i)
    {
      int grain = std::atoi(argv[i]);
      auto gpred = [&,grain] (Graph::vertex_descriptor v) {
	return
	(glink[v][0] != t1.nend() and a1[glink[v][0]] > grain) or
	(glink[v][1] != t2.nend() and a2[glink[v][1]] > grain) or
	(glink[v][2] != t3.nend() and a3[glink[v][2]] > grain);
      };

      boost::filtered_graph<Graph, boost::keep_all, std::function<bool(Graph::vertex_descriptor)> > fg(g2, boost::keep_all(), gpred);


      reconstruct_marginal(t1, t2, t3, val1, val2, val3, fg, out);
      copy(out | sbox2d{out.domain().pmin + 2, out.domain().pmax - 2, point2d{2,2}}, tmp);
      io::imsave(tmp, (boost::format("out-%04i.tiff") % grain).str().c_str());
    }
}
