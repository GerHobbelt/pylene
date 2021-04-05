#include "mln/hierarchies/segmentation.hpp"

#include "mln/hierarchies/attributes.hpp"


namespace mln
{
  static std::vector<int> get_hierarchy_attribute(const QBT& qbt, WatershedAttribute attribute_type)
  {
    std::vector<int> attribute;

    switch (attribute_type)
    {
    case AREA:
      attribute = area_attribute(qbt);
      break;
    case DYNAMIC:
      attribute = dynamic_attribute(qbt);
      break;
    case HEIGHT:
      attribute = height_attribute(qbt);
      break;
    case DEPTH:
      attribute = depth_attribute(qbt);
      break;
    }

    return attribute;
  }

  static std::vector<int> get_qbt_computed_attribute(const Graph* leaf_graph, const QBT& qbt,
                                                     std::vector<int> attribute)
  {
    int qbt_nb_vertices = qbt.get_nb_vertices();
    int qbt_root        = qbt_nb_vertices - 1;

    std::vector<int> qbt_computed_attribute(qbt_nb_vertices, std::numeric_limits<int>::min());
    std::fill_n(qbt_computed_attribute.begin(), leaf_graph->get_nb_vertices(), 0);

    for (int node = 0; node < qbt_root; ++node)
    {
      int parent_node = qbt.get_parent(node);

      if (node >= leaf_graph->get_nb_vertices() &&
          leaf_graph->weight_node(parent_node) != leaf_graph->weight_node(node))
      {
        qbt_computed_attribute[node] = attribute[node];
      }

      qbt_computed_attribute[parent_node] = std::max(qbt_computed_attribute[parent_node], qbt_computed_attribute[node]);
    }

    qbt_computed_attribute[qbt_root] = attribute[qbt_root];

    return qbt_computed_attribute;
  }

  Graph* watershed_graph(Graph* graph, WatershedAttribute attribute_type)
  {
    QEBT*      qebt = graph->kruskal();
    const QBT& qbt  = qebt->get_qbt();

    int qbt_nb_vertices = qbt.get_nb_vertices();

    std::vector<int> attribute               = get_hierarchy_attribute(qbt, attribute_type);
    std::vector<int> qbt_computed_attributes = get_qbt_computed_attribute(graph, qbt, attribute);

    std::vector<int> min_qbt_computed_attributes(qbt_nb_vertices, std::numeric_limits<int>::max());
    for (int node = 0; node < qbt_nb_vertices - 1; ++node)
    {
      min_qbt_computed_attributes[qbt.get_parent(node)] =
          std::min(min_qbt_computed_attributes[qbt.get_parent(node)], qbt_computed_attributes[node]);
    }

    auto* res = new Graph(graph->get_height(), graph->get_width());

    const std::vector<Edge>& graph_edges = graph->get_edges();

    for (int i_node = graph->get_nb_vertices(); i_node < qbt_nb_vertices; ++i_node)
    {
      const Edge& edge = graph_edges[graph->get_edge(i_node)];
      res->add_edge(std::get<0>(edge), std::get<1>(edge), min_qbt_computed_attributes[i_node]);
    }

    delete qebt;
    return res;
  }

  std::vector<int> threshold_cut_labelization(const HierarchyTree& tree, double threshold)
  {
    int          tree_nb_vertices = tree.get_nb_vertices();
    const Graph* leaf_graph       = tree.leaf_graph;

    int max_altitude = std::numeric_limits<int>::min();
    for (int i_node = leaf_graph->get_nb_vertices(); i_node < tree_nb_vertices; ++i_node)
      max_altitude = std::max(max_altitude, leaf_graph->weight_node(i_node));

    std::vector<int> labels(tree_nb_vertices, -1);

    for (int i_node = tree_nb_vertices - 2; i_node >= leaf_graph->get_nb_vertices(); --i_node)
    {
      int parent_node = tree.get_parent(i_node);
      if (parent_node == -1)
        continue;

      labels[i_node] = (leaf_graph->weight_node(parent_node) / static_cast<double>(max_altitude)) > threshold
                           ? i_node
                           : labels[parent_node];
    }

    for (int leaf = 0; leaf < leaf_graph->get_nb_vertices(); ++leaf)
      labels[leaf] = labels[tree.get_parent(leaf)];

    return labels;
  }
} // namespace mln