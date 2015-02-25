#include <mln/core/neighb2d.hpp>
#include <mln/core/dontcare.hpp>
#include <mln/morpho/tos/ctos.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>


#include <apps/tos/addborder.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/topology.hpp>
#include "compute_g2.hpp"
#include "satmaxtree.hpp"

#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <tbb/parallel_for.h>

namespace mln
{

  template <class MyGraph>
  boost::vector_property_map<unsigned>
  compute_graph_depth(const MyGraph& g)
  {
    mln_entering("Compute graph depth");

    boost::vector_property_map<unsigned> depth(boost::num_vertices(g));

    auto one = [](mln::dontcare_t) -> int{ return 1; };
    auto w = boost::make_function_property_map<typename MyGraph::edge_descriptor, int, decltype(one)>(one);

    typename MyGraph::vertex_descriptor root = boost::vertex(0, g);
    depth[root] = 0;

    MyGraph gT;
    boost::transpose_graph(g, gT);
    boost::dag_shortest_paths(gT, root, boost::weight_map(w)
                              .distance_map(depth)
                              .distance_compare(std::greater<int> ())
                              .distance_inf(-1)
                              .distance_zero(0)
                            );


    mln_exiting();
    return depth;
  }

  template <class MyGraph, class ValueMap, class BinaryFunction, class ValueType>
  void
  write_vmap_to_image(const MyGraph& g, const tree_t* t, const tlink_t* tlink,
                      const ValueMap& vmap, BinaryFunction op, ValueType init,
                      image2d<ValueType>& out)
  {
    (void) g;

    mln_pixter(px, out);
    mln_forall(px)
    {
      ValueType w = init;
      for (int i = 0; i < NTREE; ++i)
        {
          tree_t::node_type tnode = t[i].get_node_at(px->index());
          typename MyGraph::vertex_descriptor gnode = tlink[i][tnode];
          w = op(w, vmap[gnode]);
        }
      px->val() = w;
    }

  }


  template <class V>
  tree_t
  compute_ctos(const image2d<V>& input,
               image2d<uint16>* imdepth)
  {
    typedef V value_t;
    enum { NTREE = value_t::ndim };


    image2d<value_t> f = addborder_marginal(input);
    image2d<value_t> F = immerse_k1(f);
    image2d<uint16> maxdepth;
    point2d pmin = {0,0};

    {
      /// Compute the marginal tree
      tree_t t[NTREE];
      tbb::parallel_for(0, (int)NTREE, [&t,&f,pmin](int i){
          t[i] = morpho::cToS_pinf(imtransform(f, [i](value_t x) {
                return x[i]; }), c4, pmin);
        });


      /// Compute the graph
      Graph<NTREE> g2;
      std::array<property_map<tree_t, typename MyGraph::vertex_descriptor>, NTREE> tlink;
      std::tie(g2, tlink) = compute_g2<NTREE>(t);


      /// Compute depth
      boost::vector_property_map<unsigned> gdepth;
      gdepth = compute_graph_depth(g2);


      /// Compute the pw depth image from graph
      resize(maxdepth, F);
      write_vmap_to_image(g2, t, &tlink[0], gdepth,
                          functional::max_t<unsigned> (), uint16(0), maxdepth);
    }

    /// Compute the saturated maxtree
    tree_t tw;
    property_map<tree_t, uint16> depth2;
    std::tie(tw, depth2) = satmaxtree(maxdepth, pmin);

    /// Remove the 2F from the tree
    tw = tree_keep_2F(tw);

    /// Outputs the depth image
    if (imdepth) {
      resize(*imdepth, maxdepth);
      morpho::reconstruction(tw, depth2, *imdepth);
    }
    return tw;
  }


   tree_t compute_ctos_from_maxtrees(const image2d<rgb8>& input,
                                     image2d<uint16>* imdepth,
                                     bool mintree)
   {
     typedef rgb8 value_t;
     enum { NTREE = value_t::ndim };


     image2d<value_t> f = input; //addborder_marginal(input);
     image2d<value_t> F = input; //immerse_k1(f);


     /// Compute the marginal tree
     tree_t t[NTREE];
     tbb::parallel_for(0, (int)NTREE, [&t,&f,mintree](int i){
         if (mintree)
           t[i] = morpho::mintree_indexes(eval(channel(f, i)), c4);
         else
           t[i] = morpho::maxtree_indexes(eval(channel(f, i)), c4);
       });

     /// Compute the graph
     Graph<NTREE> g2;
     std::array<property_map<tree_t, typename MyGraph::vertex_descriptor>, NTREE> tlink;
     std::tie(g2, tlink) = compute_g2<NTREE>(t);


     /// Compute depth
     boost::vector_property_map<unsigned> gdepth;
     gdepth = compute_graph_depth(g2);


     /// Compute the pw depth image from graph
     image2d<uint16> maxdepth;
     resize(maxdepth, F);
     write_vmap_to_image(g2, t, &tlink[0], gdepth,
                         functional::max_t<unsigned> (), uint16(0), maxdepth);

     /// Compute the saturated maxtree
     tree_t tw;
     tw = morpho::maxtree_indexes(maxdepth, c4);

     /// Outputs the depth image
     if (imdepth) {
       *imdepth = maxdepth;
     }
     return tw;
   }

  // Explicit instanciation.
  template
  tree_t
  compute_ctos<rgb8>(const image2d<rgb8>& input,
                     image2d<uint16>* imdepth);

  // Explicit instanciation.
  template
  tree_t
  compute_ctos<rgb16>(const image2d<rgb16>& input,
                      image2d<uint16>* imdepth);

}
