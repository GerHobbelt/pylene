#pragma once

#include "mln/hierarchies/hierarchy_tree.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/core/image/ndimage_fwd.hpp"

namespace mln
{
    std::vector<Edge> saliency_map(const HierarchyTree& tree);

    // FIXME Return an image
    unsigned* saliency_khalimsky_grid(const HierarchyTree& tree);

} // namespace mln