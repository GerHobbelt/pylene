#include "compute_ctos.hpp"

namespace mln
{

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
                     image2d<uint16>* imdepth,
                     e_ctos_attribute attribute,
                     ctos_extra_params_t params
                     );

  // Explicit instanciation.
  template
  tree_t
  compute_ctos<rgb16>(const image2d<rgb16>& input,
                      image2d<uint16>* imdepth,
                      e_ctos_attribute attribute,
                      ctos_extra_params_t params
                      );

}
