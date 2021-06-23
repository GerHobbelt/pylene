#pragma once

#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/component_tree.hpp>

namespace mln::morpho
{
  /// Compute the watershed hierarchy of an image
  ///
  /// \param input The input image
  /// \param attribute_func The function that define the attribute computation
  /// \param neighborhood The neighborhood relation
  /// \param distance Distance function
  template <class I, class A, class N, class F = mln::functional::l2dist_t<>>
  std::pair<component_tree<typename std::invoke_result_t<
                A, component_tree<std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>,
                image_ch_value_t<I, int>>::value_type>,
            image_ch_value_t<I, int>> //
  watershed_hierarchy(I input, A attribute_func, N nbh, F distance = F{});


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace internal
  {
    template <typename W, typename A>
    std::vector<A> get_computed_attribute(const component_tree<W>& tree, const std::vector<A>& attribute, int nb_leaves)
    {
      int n = static_cast<int>(tree.parent.size());

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
      auto computed_attribute = get_computed_attribute(tree, attribute, static_cast<int>(nb_leaves));

      int n = static_cast<int>(tree.parent.size());

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

  template <typename W, class I>
  std::vector<W> height_attribute(component_tree<W> tree, I node_map)
  {
    int n         = static_cast<int>(tree.parent.size());
    int nb_leaves = static_cast<int>(node_map.domain().size());

    std::vector<W> deepest_altitude(n, std::numeric_limits<W>::max());
    for (int i = n - 1; i >= 0; --i)
    {
      int parent = tree.parent[i];

      if (i > (n - nb_leaves - 1))
        deepest_altitude[i] = tree.values[parent];

      deepest_altitude[parent] = std::min(deepest_altitude[parent], deepest_altitude[i]);
    }

    std::vector<W> height(n);
    for (int i = n - 1; i >= 0; --i)
      height[i] = tree.values[tree.parent[i]] - deepest_altitude[i];

    return height;
  }

  template <typename W, class I>
  std::vector<bool> extrema_attribute(component_tree<W> tree, I node_map)
  {
    int n         = static_cast<int>(tree.parent.size());
    int nb_leaves = static_cast<int>(node_map.domain().size());

    std::vector<bool> extrema(n, true);
    std::fill_n(extrema.end() - nb_leaves, nb_leaves, false);

    for (int i = (n - nb_leaves - 1); i > 0; --i)
    {
      int parent = tree.parent[i];

      bool same_weight = tree.values[i] == tree.values[parent];
      extrema[parent]  = extrema[parent] && same_weight && extrema[i];
      extrema[i]       = extrema[i] && !same_weight;
    }

    return extrema;
  }

  template <typename W, class I>
  std::vector<W> dynamic_attribute(component_tree<W> tree, I node_map)
  {
    int n         = static_cast<int>(tree.parent.size());
    int nb_leaves = static_cast<int>(node_map.domain().size());

    std::vector<W>   deepest_altitude(n, std::numeric_limits<W>::max());
    std::vector<int> path_to_minima(n, -1);

    // Compute deepest altitude and path to deepest minima
    for (int i = (n - nb_leaves - 1); i >= 0; --i)
    {
      int parent = tree.parent[i];

      // Deepest non leaf node
      if (deepest_altitude[i] == std::numeric_limits<W>::max())
        deepest_altitude[i] = tree.values[i];

      if (deepest_altitude[i] < deepest_altitude[parent])
      {
        deepest_altitude[parent] = deepest_altitude[i];
        path_to_minima[parent]   = i;
      }
    }

    auto             extrema = extrema_attribute(tree, node_map);
    std::vector<int> nearest_minima(n, -1);

    std::vector<W> dynamic(n);
    dynamic[0] = tree.values[0] - deepest_altitude[0];

    for (int i = 1; i < n; ++i)
    {
      int parent = tree.parent[i];

      if (i > (n - nb_leaves - 1))
      {
        if (nearest_minima[parent] != -1)
          dynamic[i] = dynamic[nearest_minima[parent]];
        else
          dynamic[i] = 0;

        continue;
      }

      if (extrema[i])
        nearest_minima[i] = i;
      else
        nearest_minima[i] = nearest_minima[parent];

      if (i == path_to_minima[parent])
        dynamic[i] = dynamic[parent];
      else
        dynamic[i] = tree.values[parent] - deepest_altitude[i];
    }

    return dynamic;
  }

  template <class I, class A, class N, class F>
  std::pair<component_tree<typename std::invoke_result_t<
                A, component_tree<std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>,
                image_ch_value_t<I, int>>::value_type>,
            image_ch_value_t<I, int>> //
  watershed_hierarchy(I input, A attribute_func, N nbh, F distance)
  {
    std::vector<edge_t<image_point_t<I>, std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>> mst;
    auto [tree, nm] = internal::__alphatree<false>(input, nbh, distance, false, false, &mst);

    auto attribute = attribute_func(tree, nm);

    auto node_count = tree.parent.size();
    mln::for_each(nm, [node_count](int& id) { id = static_cast<int>(node_count) - id - 1; });

    return internal::watershed<I, N>(tree, nm, mst, attribute, input.domain().size());
  }
} // namespace mln::morpho