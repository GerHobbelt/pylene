#include "mln/hierarchies/attributes.hpp"
#include "mln/hierarchies/graph.hpp"

#include "mln/hierarchies/accumulators/deepest_altitude_accumulator.hpp"
#include "mln/hierarchies/accumulators/depth_accumulator.hpp"
#include "mln/hierarchies/accumulators/extrema_accumulator.hpp"
#include "mln/hierarchies/accumulators/height_accumulator.hpp"
#include "mln/hierarchies/accumulators/sum_accumulator.hpp"
#include "mln/hierarchies/accumulators/volume_accumulator.hpp"

#include <numeric>


namespace mln
{
  std::vector<int> hierarchy_traversal(const HierarchyTree& tree, const HierarchyTraversal& traversal)
  {
    // Exclude root
    std::vector<int> res((traversal.exclude_leaves ? tree.get_nb_vertices() / 2 : tree.get_nb_vertices()) - 1);
    std::iota(res.begin(), res.end(), traversal.exclude_leaves ? tree.leaf_graph.get_nb_vertices() : 0);

    if (traversal.order == HierarchyTraversal::ROOT_TO_LEAVES)
      std::reverse(res.begin(), res.end());

    return res;
  }

  template <typename T, Accumulator<T> AccumulatorType>
  std::vector<T> compute_attribute_from_accumulator(const HierarchyTree& tree, const AccumulatorType& acc,
                                                    const HierarchyTraversal& traversal, std::vector<T> init_list)
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<AccumulatorType> accumulators(tree_nb_vertices, acc);
    for (int i = 0; i < tree_nb_vertices; ++i)
      accumulators[i].set_associated_node(i);

    for (size_t i = 0; i < init_list.size(); ++i)
      accumulators[i].init(init_list[i]);

    std::vector<int> traversal_vector = hierarchy_traversal(tree, traversal);

    if (traversal.order == HierarchyTraversal::LEAVES_TO_ROOT)
    {
      for (const auto& node : traversal_vector)
      {
        int parent_node = tree.get_parent(node);
        if (parent_node == -1)
        {
          accumulators[node].invalidate();
          continue;
        }

        accumulators[parent_node].merge(accumulators[node]);
      }
    }
    else if (traversal.order == HierarchyTraversal::ROOT_TO_LEAVES)
    {
      for (const auto& node : traversal_vector)
      {
        int parent_node = tree.get_parent(node);
        if (parent_node == -1)
        {
          accumulators[node].invalidate();
          continue;
        }

        accumulators[node].merge(accumulators[parent_node]);
      }
    }

    std::vector<T> attribute(tree_nb_vertices);

    for (int i = 0; i < tree_nb_vertices; ++i)
      attribute[i] = accumulators[i].get_value();

    return attribute;
  }

  std::vector<int> depth_attribute(const HierarchyTree& tree)
  {
    return compute_attribute_from_accumulator<int>(tree, DepthAccumulator(),
                                                   HierarchyTraversal{HierarchyTraversal::ROOT_TO_LEAVES});
  }

  std::vector<int> area_attribute(const HierarchyTree& tree)
  {
    std::vector<int> one_leaves(tree.leaf_graph.get_nb_vertices(), 1);
    return compute_attribute_from_accumulator<int>(tree, SumAccumulator(), HierarchyTraversal{}, one_leaves);
  }

  std::vector<int> volume_attribute(const HierarchyTree& tree)
  {
    int nb_leaves = tree.leaf_graph.get_nb_vertices();

    auto diff_altitude = [&](int node) {
      if (node < nb_leaves)
        return 0;

      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        return -1;

      return tree.leaf_graph.weight_node(parent_node) - tree.leaf_graph.weight_node(node);
    };

    std::vector<int> one_leaves(tree.leaf_graph.get_nb_vertices(), 1);

    return compute_attribute_from_accumulator<int>(tree, VolumeAccumulator(diff_altitude),
                                                   HierarchyTraversal{HierarchyTraversal::LEAVES_TO_ROOT}, one_leaves);
  }

  std::vector<int> extrema_attribute(const HierarchyTree& tree)
  {
    auto same_altitude = [&](int node) {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        return false;

      return tree.leaf_graph.weight_node(parent_node) == tree.leaf_graph.weight_node(node);
    };

    std::vector<int> zero_leaves(tree.leaf_graph.get_nb_vertices(), 0);

    return compute_attribute_from_accumulator<int>(tree, ExtremaAccumulator(same_altitude),
                                                   HierarchyTraversal{HierarchyTraversal::LEAVES_TO_ROOT, true},
                                                   zero_leaves);
  }

  std::vector<int> height_attribute(const HierarchyTree& tree)
  {
    auto parent_altitude = [&](int node) {
      // Root case
      if (node == tree.get_nb_vertices() - 1)
        return tree.leaf_graph.weight_node(node);

      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        return -1;

      return tree.leaf_graph.weight_node(parent_node);
    };

    std::vector<int> parent_altitude_node(tree.leaf_graph.get_nb_vertices());
    for (int leaf = 0; leaf < tree.leaf_graph.get_nb_vertices(); ++leaf)
      parent_altitude_node[leaf] = parent_altitude(leaf);

    return compute_attribute_from_accumulator<int>(tree, HeightAccumulator(parent_altitude),
                                                   HierarchyTraversal{HierarchyTraversal::LEAVES_TO_ROOT},
                                                   parent_altitude_node);
  }

  std::vector<int> dynamic_attribute(const HierarchyTree& tree)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    auto internal_altitude = [&](int node) { return tree.leaf_graph.weight_node(node); };

    std::vector<std::tuple<int, int>> zero_leaves(tree.leaf_graph.get_nb_vertices(), std::make_tuple<>(0, -1));

    std::vector<std::tuple<int, int>> deepest_altitude = compute_attribute_from_accumulator<std::tuple<int, int>>(
        tree, DeepestAltitudeAccumulator(internal_altitude),
        HierarchyTraversal{HierarchyTraversal::LEAVES_TO_ROOT, true}, zero_leaves);

    int root = tree_nb_vertices - 1;

    std::vector<int> dynamic(tree_nb_vertices, -1);
    dynamic[root] = leaf_graph.weight_node(root) - std::get<0>(deepest_altitude[root]);

    std::vector<int> minima = extrema_attribute(tree);
    std::vector<int> nearest_minima(tree_nb_vertices, -1);

    for (int node = root - 1; node >= 0; --node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
      {
        dynamic[node] = -1;
        continue;
      }

      if (node < leaf_graph.get_nb_vertices())
      {
        if (nearest_minima[parent_node] != -1)
          dynamic[node] = dynamic[nearest_minima[parent_node]];
        else
          dynamic[node] = 0;

        continue;
      }

      if (minima[node])
        nearest_minima[node] = node;
      else
        nearest_minima[node] = nearest_minima[parent_node];

      if (node == std::get<1>(deepest_altitude[parent_node]))
        dynamic[node] = dynamic[parent_node];
      else
        dynamic[node] = leaf_graph.weight_node(parent_node) - std::get<0>(deepest_altitude[node]);
    }

    return dynamic;
  }

} // namespace mln