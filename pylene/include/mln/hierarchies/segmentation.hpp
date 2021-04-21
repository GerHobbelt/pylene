#pragma once

#include "mln/core/colors.hpp"
#include "mln/core/image/ndbuffer_image.hpp"
#include "mln/hierarchies/accumulators/max_accumulator.hpp"
#include "mln/hierarchies/accumulators/sum_accumulator.hpp"
#include "mln/hierarchies/attributes.hpp"
#include "mln/hierarchies/graph.hpp"


namespace mln
{
  /**
   * Get the attribute at the time of the merging on the Binary Partition Tree
   *
   * @tparam AttributeType Attribute Type
   *
   * @param qbt The Binary Partition Tree where we flood the attribute
   * @param attribute Attribute computed on the Binary Partition Tree
   */
  template <Attribute AttributeType>
  inline std::vector<AttributeType> get_qbt_computed_attribute(const QBT& qbt, std::vector<AttributeType> attribute)
  {
    const Graph& leaf_graph = qbt.leaf_graph;

    int qbt_nb_vertices   = qbt.get_nb_vertices();
    int graph_nb_vertices = leaf_graph.get_nb_vertices();
    int qbt_root          = qbt_nb_vertices - 1;

    std::vector<AttributeType> leaves(graph_nb_vertices, 0);

    std::vector<AttributeType> edges(qbt_nb_vertices - graph_nb_vertices, std::numeric_limits<AttributeType>::min());
    for (int i_node = leaf_graph.get_nb_vertices(); i_node < qbt_nb_vertices - 1; ++i_node)
    {
      if (leaf_graph.weight_node(qbt.get_parent(i_node)) != leaf_graph.weight_node(i_node))
        edges[i_node - graph_nb_vertices] = attribute[i_node];
    }

    edges[qbt_root - graph_nb_vertices] = attribute[qbt_root];

    return compute_attribute_from_accumulator<AttributeType, AttributeType>(qbt, MaxAccumulator<AttributeType>(),
                                                                            leaves, edges);
  }

  /**
   * Compute the watershed graph from an initial adjacency graph and an attribute function
   * To obtain the second hierarchy, the kruskal method have to be called on the watershed graph
   *
   * @tparam AttributeType Attribute Type
   *
   * @param graph Initial adjacency graph computed on an image
   * @param attribute_func Attribute function
   */
  template <Attribute AttributeType>
  inline Graph watershed_graph(Graph&                                                                 graph,
                               const std::function<std::vector<AttributeType>(const HierarchyTree&)>& attribute_func)
  {
    const QEBT& qebt = graph.kruskal();
    const QBT&  qbt  = qebt.get_qbt();

    int qbt_nb_vertices = qbt.get_nb_vertices();

    std::vector<AttributeType> attribute               = attribute_func(qbt);
    std::vector<AttributeType> qbt_computed_attributes = get_qbt_computed_attribute<>(qbt, attribute);

    std::vector<AttributeType> min_qbt_computed_attributes(qbt_nb_vertices, std::numeric_limits<AttributeType>::max());
    for (int node = 0; node < qbt_nb_vertices - 1; ++node)
    {
      min_qbt_computed_attributes[qbt.get_parent(node)] =
          std::min(min_qbt_computed_attributes[qbt.get_parent(node)], qbt_computed_attributes[node]);
    }

    Graph res(graph.get_height(), graph.get_width());

    const std::vector<Edge>& graph_mst = graph.mst;

    for (int i_node = graph.get_nb_vertices(); i_node < qbt_nb_vertices; ++i_node)
    {
      const Edge& edge = graph_mst[graph.get_edge(i_node)];
      res.add_edge(std::get<0>(edge), std::get<1>(edge), min_qbt_computed_attributes[i_node]);
    }

    return res;
  }

  /**
   * Label all nodes with the nearest connected component root of the tree they belong
   * determined by the cut of the Hierarchy Tree with a threshold
   *
   * @param tree Hierarchy Tree were the cut will be performed
   * @param threshold Value in [0, 1] that represent the altitude percentage of the cut
   *
   * @return The nearest connected component root for each node
   */
  inline std::vector<int> threshold_cut_labelization(const HierarchyTree& tree, double threshold)
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

      labels[i_node] = (leaf_graph.weight_node(parent_node) / max_altitude) > threshold ? i_node : labels[parent_node];
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
  inline std::vector<rgb8> mean_color_per_node(const HierarchyTree& tree, const image2d<rgb8>& image)
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

  /**
   * General function that encapsulate the flow of hierarchical segmentations
   *
   * @tparam AttributeType Attribute Type
   *
   * @param image Image where the hierarchical segmentation will be applied
   * @param attribute_func Attribute function that will define the second hierarchy (used to reduce over-segmentation)
   * @param threshold Value in [0, 1] that represent the wanted number of details in the resulting image
   *                  The lower the threshold, the greater the details on the resulting image
   *
   * @return Segmented image
   */
  template <Attribute AttributeType>
  inline image2d<rgb8>
  hierarchical_segmentation(const mln::image2d<rgb8>&                                              image,
                            const std::function<std::vector<AttributeType>(const HierarchyTree&)>& attribute_func,
                            double                                                                 threshold)
  {
    Graph graph(image);
    Graph watershed_graph = mln::watershed_graph<AttributeType>(graph, attribute_func);

    const HierarchyTree& tree = watershed_graph.kruskal();

    // FIXME threshold has to be unexpectedly low to obtain good images
    //       It should should be a value in [0, 1] that represent a percentage
    std::vector<int>  labels     = threshold_cut_labelization(tree, threshold);
    std::vector<rgb8> mean_color = mean_color_per_node(tree, image);

    mln::image2d<rgb8> res(image.width(), image.height());

    int width = image.width();

    for (int leaf = 0; leaf < tree.leaf_graph.get_nb_vertices(); ++leaf)
      res({leaf % width, leaf / width}) = mean_color[labels[leaf]];

    return res;
  }
} // namespace mln