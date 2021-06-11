#pragma once

#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/component_tree.hpp>

namespace mln::morpho
{
  // FIXME Pass a std::function instead of an accumulator ?

  /// Compute the watershed hierarchy of an image
  ///
  /// \param input The input image
  /// \param acc The accumulator that define the attribute computation
  /// \param neighborhood The neighborhood relation
  /// \param distance Distance function
  template <class I, class Accu, class N, class F = mln::functional::l2dist_t<>>
  std::pair<component_tree<typename accu::result_of<Accu, image_point_t<I>>::type>, image_ch_value_t<I, int>> //
  watershed_hierarchy(I input, Accu acc, N nbh, F distance = F{});


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace internal
  {
    template <typename W, typename A>
    std::vector<A> get_computed_attribute(const component_tree<W>& tree, const std::vector<A>& attribute, int nb_leaves)
    {
      int n = tree.parent.size();

      std::vector<A> res(n, std::numeric_limits<A>::min());
      std::fill_n(res.end() - nb_leaves, nb_leaves, 0);

      for (int i = n - 1; i > 0; --i)
      {
        int parent = tree.parent[i];

        if (i <= (n - nb_leaves - 1) && tree.values[parent] != tree.values[i])
          res[i] = attribute[i];

        res[parent] = std::max(res[parent], res[i]);
      }
      res[0] = attribute[0];

      return res;
    }

    template <typename I, typename N, typename W, typename E, typename A>
    std::pair<component_tree<A>, image_ch_value_t<I, int>>
    watershed(const component_tree<W>& tree, image_ch_value_t<I, int> node_map, std::vector<E> mst,
              const std::vector<A>& attribute, std::size_t nb_leaves)
    {
      auto computed_attribute = get_computed_attribute(tree, attribute, nb_leaves);

      int n = tree.parent.size();

      std::vector<A> min_computed_attributes(n, std::numeric_limits<A>::max());
      for (int i = n - 1; i > 0; --i)
      {
        int parent = tree.parent[i];

        min_computed_attributes[parent] = std::min<A>(min_computed_attributes[parent], computed_attribute[i]);
      }

      auto edges = alphatree_edges<image_point_t<I>, N, A, false>();
      for (std::size_t i = 0; i < mst.size(); ++i)
      {
        auto [p, q, _] = mst[i];
        A new_weight   = min_computed_attributes[nb_leaves - i - 2];
        edges.push(p, q, new_weight);
      }
      edges.on_finish_insert();

      return internal::alphatree_from_graph<A>(edges, node_map, nb_leaves, true);
    }
  } // namespace internal

  template <class I, class Accu, class N, class F>
  std::pair<component_tree<typename accu::result_of<Accu, image_point_t<I>>::type>, image_ch_value_t<I, int>> //
  watershed_hierarchy(I input, Accu acc, N nbh, F distance)
  {
    std::vector<internal::edge_t<image_point_t<I>, std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>> mst;
    auto [tree, nm] = internal::__alphatree<I, N, F, false>(input, nbh, distance, false, false, &mst);

    auto attribute = tree.compute_attribute_on_points(nm, acc);

    auto node_count = tree.parent.size();
    mln::for_each(nm, [node_count](int& id) { id = static_cast<int>(node_count) - id - 1; });

    return internal::watershed<I, N>(tree, nm, mst, attribute, input.domain().size());
  }
} // namespace mln::morpho