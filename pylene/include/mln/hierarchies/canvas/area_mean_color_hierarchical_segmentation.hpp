#pragma once

#include "mln/core/colors.hpp"
#include "mln/hierarchies/accumulators/sum_accumulator.hpp"
#include "mln/hierarchies/attributes.hpp"
#include "mln/hierarchies/canvas/hierarchical_segmentation_base.hpp"

namespace mln
{
  struct AreaMeanColorHierarchicalSegmentation : public HierarchicalSegmentationBase<int, rgb8>
  {
    std::vector<int> on_attribute_computation(const HierarchyTree& tree) override { return area_attribute(tree); }

    /**
     * Label all nodes with the nearest connected component root of the tree they belong
     * determined by the cut of the Hierarchy Tree with a threshold
     *
     * @param tree Hierarchy Tree were the cut will be performed
     * @param altitude_threshold Value in [0, 1] that represent the altitude percentage of the cut
     *
     * @return The nearest connected component root for each node
     */
    std::vector<int> on_labelization(const HierarchyTree& tree, double altitude_threshold) override
    {
      int          tree_nb_vertices = tree.get_nb_vertices();
      const Graph& leaf_graph       = tree.leaf_graph;

      double max_altitude = std::numeric_limits<double>::min();
      for (int i_node = leaf_graph.get_nb_vertices(); i_node < tree_nb_vertices; ++i_node)
        max_altitude = std::max(max_altitude, leaf_graph.weight_node(i_node));

      std::vector<int> labels(tree_nb_vertices, -1);

      for (int i_node = tree_nb_vertices - 2; i_node >= leaf_graph.get_nb_vertices(); --i_node)
      {
        int parent_node = tree.get_parent(i_node);
        if (parent_node == -1)
          continue;

        labels[i_node] =
            (leaf_graph.weight_node(parent_node) / max_altitude) > altitude_threshold ? i_node : labels[parent_node];
      }

      for (int leaf = 0; leaf < leaf_graph.get_nb_vertices(); ++leaf)
        labels[leaf] = labels[tree.get_parent(leaf)];

      return labels;
    }

    /**
     * Compute the mean color per node
     *
     * @param tree Hierarchy Tree
     * @param image Initial image from which the Hierarchy Tree has been computed
     */
    std::vector<rgb8> on_reconstruction(const HierarchyTree& tree, image2d<rgb8> image) override
    {
      int          tree_nb_vertices = tree.get_nb_vertices();
      const Graph& leaf_graph       = tree.leaf_graph;

      int width = image.width();

      std::vector<rgb<unsigned long long>> colors(leaf_graph.get_nb_vertices(), rgb<unsigned long long>{0, 0, 0});

      for (int leaf = 0; leaf < leaf_graph.get_nb_vertices(); ++leaf)
        colors[leaf] = image({leaf % width, leaf / width});

      std::vector<rgb<unsigned long long>> mean_color =
          compute_attribute_from_accumulator<rgb<unsigned long long>, rgb<unsigned long long>>(
              tree, SumAccumulator<rgb<unsigned long long>>(), colors);

      std::vector<int> area = area_attribute(tree);

      for (int i_node = leaf_graph.get_nb_vertices(); i_node < tree_nb_vertices; ++i_node)
      {
        // Deleted node
        if (area[i_node] == -1)
          continue;

        mean_color[i_node] /= area[i_node];
      }

      return std::vector<rgb8>(mean_color.begin(), mean_color.end());
    }
  };
} // namespace mln