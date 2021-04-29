#pragma once

#include "mln/core/colors.hpp"
#include "mln/core/image/ndbuffer_image.hpp"
#include "mln/hierarchies/accumulators/max_accumulator.hpp"
#include "mln/hierarchies/accumulators/sum_accumulator.hpp"
#include "mln/hierarchies/canvas/hierarchical_segmentation_base.hpp"
#include "mln/hierarchies/graph.hpp"


namespace mln
{
  /**
   * Get the attribute at the time of the merging
   *
   * @tparam AttributeType Attribute Type
   *
   * @param tree The Hierarchy Tree where we flood the attribute
   * @param attribute Attribute computed on the Hierarchy Tree
   */
  template <Attribute AttributeType>
  inline std::vector<AttributeType> get_computed_attribute(const HierarchyTree&       tree,
                                                           std::vector<AttributeType> attribute)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices  = tree.get_nb_vertices();
    int graph_nb_vertices = leaf_graph.get_nb_vertices();
    int tree_root         = tree_nb_vertices - 1;

    std::vector<AttributeType> leaves(graph_nb_vertices, 0);

    std::vector<AttributeType> edges(tree_nb_vertices - graph_nb_vertices, std::numeric_limits<AttributeType>::min());
    for (int i_node = leaf_graph.get_nb_vertices(); i_node < tree_nb_vertices - 1; ++i_node)
    {
      if (leaf_graph.weight_node(tree.get_parent(i_node)) != leaf_graph.weight_node(i_node))
        edges[i_node - graph_nb_vertices] = attribute[i_node];
    }

    edges[tree_root - graph_nb_vertices] = attribute[tree_root];

    return compute_attribute_from_accumulator<AttributeType, AttributeType>(tree, MaxAccumulator<AttributeType>(),
                                                                            leaves, edges);
  }

  /**
   * Compute the watershed graph from an initial adjacency graph, the corresponding hierarchy tree and an attribute
   * To obtain the second hierarchy, the kruskal method have to be called on the watershed graph
   *
   * @tparam AttributeType Attribute Type
   *
   * @param graph Initial adjacency graph computed on an image
   * @param tree Hierarchy Tree computed on the graph with kruskal algorithm
   * @param attribute Attribute computed on the Hierarchy Tree
   */
  template <Attribute AttributeType>
  inline Graph watershed_graph(Graph& graph, const HierarchyTree& tree, std::vector<AttributeType> attribute)
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<AttributeType> tree_computed_attributes = get_computed_attribute<>(tree, attribute);

    std::vector<AttributeType> min_tree_computed_attributes(tree_nb_vertices,
                                                            std::numeric_limits<AttributeType>::max());
    for (int node = 0; node < tree_nb_vertices - 1; ++node)
    {
      min_tree_computed_attributes[tree.get_parent(node)] =
          std::min(min_tree_computed_attributes[tree.get_parent(node)], tree_computed_attributes[node]);
    }

    Graph res(graph.get_height(), graph.get_width());

    const std::vector<Edge>& graph_mst = graph.mst;

    for (int i_node = graph.get_nb_vertices(); i_node < tree_nb_vertices; ++i_node)
    {
      const Edge& edge = graph_mst[graph.get_edge(i_node)];
      res.add_edge(std::get<0>(edge), std::get<1>(edge), min_tree_computed_attributes[i_node]);
    }

    return res;
  }

  /**
   * General function that encapsulate the flow of hierarchical segmentations
   *
   * @tparam AttributeType Attribute Type
   *
   * @param image Image where the hierarchical segmentation will be applied
   * @param hierarchical_segmentation_canvas Hierarchical segmentation canvas that define few step of segmentation flow
   * @param threshold Value that represent the threshold of the attribute for the horizontal cut
   *
   * @return Segmented image
   */
  template <Attribute AttributeType, HierarchicalSegmentationCanvas<AttributeType, rgb8> Canvas>
  inline image2d<rgb8> threshold_hierarchical_segmentation(const mln::image2d<rgb8>& image,
                                                           Canvas hierarchical_segmentation_canvas, double threshold)
  {
    Graph graph(image);

    const QEBT& qebt = graph.kruskal();
    const QBT&  qbt  = qebt.get_qbt();

    // FIXME Use QCT over QBT

    auto attribute = hierarchical_segmentation_canvas.on_attribute_computation(qbt);

    Graph watershed_graph = mln::watershed_graph<>(graph, qbt, attribute);

    const HierarchyTree& tree = watershed_graph.kruskal();

    std::vector<int>  labels         = hierarchical_segmentation_canvas.on_labelization(tree, threshold);
    std::vector<rgb8> reconstruction = hierarchical_segmentation_canvas.on_reconstruction(tree, image);

    mln::image2d<rgb8> res(image.width(), image.height());

    int width = image.width();

    for (int leaf = 0; leaf < tree.leaf_graph.get_nb_vertices(); ++leaf)
      res({leaf % width, leaf / width}) = reconstruction[labels[leaf]];

    return res;
  }
} // namespace mln