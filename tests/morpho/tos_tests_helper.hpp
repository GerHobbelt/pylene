#pragma once

#include <mln/core/experimental/point.hpp>
#include <mln/core/image/experimental/ndimage_fwd.hpp>
#include <mln/morpho/experimental/component_tree.hpp>


void compare_tree_to_ref(const mln::morpho::experimental::component_tree<>&            tree,
                         const mln::experimental::image2d<int>&                        node_map,
                         const mln::experimental::image2d<mln::experimental::point2d>& parent,
                         const mln::experimental::image2d<mln::experimental::point2d>& roots);

void compare_tree_to_ref(const mln::morpho::experimental::component_tree<>&            tree,
                         const mln::experimental::image3d<int>&                        node_map,
                         const mln::experimental::image3d<mln::experimental::point3d>& parent,
                         const mln::experimental::image3d<mln::experimental::point3d>& roots);
