#ifndef MLN_APPS_TOS_CROUTINES_HPP
# define MLN_APPS_TOS_CROUTINES_HPP

# include <apps/tos/topology.hpp>

namespace mln
{

  template <class P, class Amap>
  typename morpho::component_tree<P, Amap>::node_type
  lca(const morpho::component_tree<P, Amap>& tree,
      const property_map<morpho::component_tree<P, Amap>, unsigned>& depth,
      typename morpho::component_tree<P, Amap>::node_type x,
      typename morpho::component_tree<P, Amap>::node_type y);


  /// \brief generic function to compute an attribute on contour
  /// The tree must contains the 2F/1F
  /// \param tree The component tree _containing 1-faces and 2-faces._
  /// \param valuemap An image whose values on 1-Faces and 2-Faces will be accumulated.
  /// \param accu The accumulator (-like) to compute.
  template <class P, class I, class AccuLike>
  property_map<morpho::component_tree<P, image2d<P> >,
               typename accu::result_of<AccuLike, mln_value(I)>::type>
  compute_attribute_on_contour(const morpho::component_tree<P, image2d<P> >& tree,
                               const Image<I>& valuemap,
                               const AccumulatorLike<AccuLike>& accu);


  /*******************************/
  /*** Implementation           **/
  /*******************************/

  template <class P, class Amap>
  typename morpho::component_tree<P, Amap>::node_type
  lca(const morpho::component_tree<P, Amap>& tree,
      const property_map<morpho::component_tree<P, Amap>, unsigned>& depth,
      typename morpho::component_tree<P, Amap>::node_type x,
      typename morpho::component_tree<P, Amap>::node_type y)
  {
    if (x.id() == tree.npos())
      return y;
    if (y.id() == tree.npos())
      return x;

    while (x != y)
      {
	if (depth[x] > depth[y])
	  x = x.parent();
	else if (depth[y] > depth[x])
	  y = y.parent();
	else
	  {
	    x = x.parent();
	    y = y.parent();
	  }
      }
    return x;
  }

  template <class P, class I, class AccuLike>
  property_map<morpho::component_tree<P, image2d<P> >,
               typename accu::result_of<AccuLike, mln_value(I)>::type>
  compute_attribute_on_contour(const morpho::component_tree<P, image2d<P> >& tree,
                               const Image<I>& valuemap,
                               const AccumulatorLike<AccuLike>& acc_)
  {
    mln_entering("mln::compute_attribute_on_contour");

    mln_precondition(tree._get_data()->m_pmap.border() >= 1);

    typedef morpho::component_tree<P, image2d<P> > tree_t;
    typedef typename tree_t::node_type node_type;
    typedef typename accu::result_of<AccuLike, mln_value(I)>::type R;

    const I& f = exact(valuemap);
    auto acc = accu::make_accumulator(exact(acc_), mln_value(I) ());
    acc.init();

    property_map<tree_t, decltype(acc)> accmap(tree, acc);
    property_map<tree_t, unsigned> depth = morpho::compute_depth(tree);

    {
      mln_pixter(px, f);
      mln_iter(nx, c4(px));

      mln_forall(px)
        if (K1::is_face_2(px->point()))
          {
            node_type np = tree.get_node_at(px->index());
            mln_forall(nx)
            {
              // if nx is in the extension -> nq = tree.nend()
              node_type nq = tree.get_node_at(nx->index());
              int d = (nq.id() == tree.npos()) ? -1 : depth[nq];
              for (node_type x = np; (int) depth[x] > d and x.id() != tree.npos(); x = x.parent())
                accmap[x].take(nx->val());
            }
          }
    }

    // Store values.
    property_map<tree_t, R> out(tree);
    mln_foreach(auto x, tree.nodes())
      out[x] = accmap[x].to_result();

    mln_exiting();
    return out;
  }

}


#endif // ! MLN_APPS_TOS_CROUTINES_HPP
