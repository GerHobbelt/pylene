#pragma once

#include <mln/accu/accumulators/max.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/component_tree.hpp>

namespace mln::morpho
{
  // TODO Add the attribute parameter

  /// Compute the watershed hierarchy of an image
  ///
  /// \param input The input image
  /// \param neighborhood The neighborhood relation
  /// \param distance Distance function
  template <class I, class N, class F = mln::functional::l2dist_t<>>
  std::pair<component_tree<unsigned long>, image_ch_value_t<I, int>> //
  watershed_hierarchy(I input, N nbh, F distance = F{});


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

        if (i <= nb_leaves && tree.values[parent] != tree.values[i])
          res[i] = attribute[i];

        res[parent] = std::max(res[parent], res[i]);
      }
      res[0] = attribute[0];

      return res;
    }

    template <typename I, typename W, typename E, typename N, typename A>
    std::pair<component_tree<A>, image_ch_value_t<I, int>>
    watershed(const component_tree<W>& tree, image_ch_value_t<I, int> node_map, std::vector<E> mst,
              const std::vector<A>& attribute, N nbh, std::size_t nb_leaves)
    {
      auto computed_attribute = get_computed_attribute(tree, attribute, nb_leaves);

      int n = tree.parent.size();

      std::vector<A> min_computed_attributes(n, std::numeric_limits<A>::max());
      for (int i = n - 1; i > 0; --i)
      {
        int parent = tree.parent[i];

        min_computed_attributes[parent] = std::min<A>(min_computed_attributes[parent], computed_attribute[i]);
      }

      auto edges = alphatree_edges<image_point_t<I>, N, A>();
      for (std::size_t i = 0; i < mst.size(); ++i)
      {
        auto [p, q, _] = mst[i];
        A new_weight   = min_computed_attributes[nb_leaves - i - 2];

        int dir = 0;
        for (auto nb : nbh.after(p))
        {
          if (q == nb)
            break;

          ++dir;
        }

        edges.push(dir, new_weight, p);
      }
      edges.on_finish_insert();

      return internal::alphatree_from_graph<A>(edges, node_map, nb_leaves, true);
    }
  } // namespace internal

  template <class I, class N, class F>
  std::pair<component_tree<unsigned long>, image_ch_value_t<I, int>> //
  watershed_hierarchy(I input, N nbh, F distance)
  {
    std::vector<internal::edge_t<image_point_t<I>, std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>> mst;
    std::size_t nb_leaves;
    auto [tree, nm] = internal::__alphatree(input, nbh, distance, false, &mst, &nb_leaves);

    // FIXME Area attribute is temporary
    auto area_attribute = tree.compute_attribute_on_points(nm, mln::accu::features::count<>());

    // FIXME Maybe an option in alpha tree function is better
    auto node_count = tree.parent.size();
    mln::for_each(nm, [node_count](int& id) { id = static_cast<int>(node_count) - id - 1; });

    return internal::watershed<I>(tree, nm, mst, area_attribute, nbh, nb_leaves);
  }
} // namespace mln::morpho