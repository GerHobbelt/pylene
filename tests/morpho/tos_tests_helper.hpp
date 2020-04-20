#pragma once

#include <mln/core/point.hpp>
#include <mln/core/image/ndimage_fwd.hpp>
#include <mln/morpho/component_tree.hpp>


void compare_tree_to_ref(const mln::morpho::component_tree<>&            tree,
                         const mln::image2d<int>&                        node_map,
                         const mln::image2d<mln::point2d>& parent,
                         const mln::image2d<mln::point2d>& roots);

void compare_tree_to_ref(const mln::morpho::component_tree<>&            tree,
                         const mln::image3d<int>&                        node_map,
                         const mln::image3d<mln::point3d>& parent,
                         const mln::image3d<mln::point3d>& roots);
