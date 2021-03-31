#include "mln/hierarchies/attributes.hpp"

#include <algorithm>

namespace mln
{
  int* depth_attribute(const HierarchyTree& tree)
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    int* depth = new int[tree_nb_vertices];
    std::fill_n(depth, tree_nb_vertices, -1);

    // Set root depth to 0
    depth[tree_nb_vertices - 1] = 0;

    for (int node = tree_nb_vertices - 2; node >= 0; --node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        continue;

      depth[node] = depth[parent_node] + 1;
    }

    return depth;
  }

} // namespace mln