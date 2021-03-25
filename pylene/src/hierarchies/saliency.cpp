#include "mln/hierarchies/saliency.hpp"

#include <algorithm>

namespace mln
{
    static int* lca_preprocess(const HierarchyTree& tree)
    {
        unsigned tree_nb_vertices = tree.get_nb_vertices();

        int* depth = new int[tree_nb_vertices];
        std::fill_n(depth, tree_nb_vertices, -1);

        // Set root depth to 0
        depth[tree_nb_vertices - 1] = 0;

        for (unsigned node = tree_nb_vertices - 2; node > 0; --node)
        {
            unsigned parent_node = tree.get_parent(node);
            if (parent_node == -1)
                continue;

            depth[node] = depth[parent_node] + 1;
        }

        depth[0] = depth[tree.get_parent(0)] + 1;

        return depth;
    }

    static unsigned lca(int* depth, const HierarchyTree& tree, unsigned u, unsigned v)
    {
        // Put u and v at the same level
        while (depth[u] != depth[v])
        {
            if (depth[u] > depth[v])
                u = tree.get_parent(u);
            else
                v = tree.get_parent(v);
        }

        // Go up until the ancestor is common
        while (u != v)
        {
            u = tree.get_parent(u);
            v = tree.get_parent(v);
        }

        return u;
    }

    std::vector<Edge> saliency_map(const HierarchyTree& tree)
    {
        std::vector<Edge> res;

        const Graph& leaf_graph = tree.leaf_graph;

        int* depth = lca_preprocess(tree);

        for (Edge edge : leaf_graph.get_edges())
        {
            unsigned merge_parent_node = lca(depth, tree, std::get<0>(edge), std::get<1>(edge));
            std::get<2>(edge) = leaf_graph.weight_node(merge_parent_node);
            res.emplace_back(edge);
        }

        return res;
    }

    unsigned* saliency_khalimsky_grid(const HierarchyTree& tree)
    {
        const Graph& leaf_graph = tree.leaf_graph;

        unsigned height = leaf_graph.get_height();
        unsigned width = leaf_graph.get_width();

        unsigned res_height = 2 * height + 1;
        unsigned res_width = 2 * width + 1;

        unsigned* res = new unsigned[res_height * res_width];
        std::fill_n(res, res_height * res_width, 0);

        const std::vector<Edge>& s_map = saliency_map(tree);

        for (const auto& edge : leaf_graph.get_edges())
        {
            unsigned u = std::get<0>(edge);
            unsigned v = std::get<1>(edge);
            unsigned w = std::get<2>(edge);

            int u_pos[2] = { u / width, u % width };
            int v_pos[2] = { v / width, v % width };

            int res_offset[2] = { u_pos[0] - v_pos[0], u_pos[1] - v_pos[1] };

            int res_edge_pos[2] = { 2 * v_pos[0] + 1 + res_offset[0],
                                    2 * v_pos[1] + 1 + res_offset[1] };

            res[res_edge_pos[0] * width + res_edge_pos[1]] = w;
        }

        for (int y = 0; y < height; y += 2)
        {
            for (int x = 0; x < width; x += 2)
            {
                unsigned max = std::numeric_limits<unsigned>::min();

                if (y + 1 < height)
                    max = std::max(max, res[(y + 1) * width + x]);
                if (x + 1 < width)
                    max = std::max(max, res[y * width + x + 1]);
                if (y - 1 >= 0)
                    max = std::max(max, res[(y - 1) * width + x]);
                if (x - 1 >= 0)
                    max = std::max(max, res[y * width + x - 1]);

                res[y * width + x] = max;
            }
        }

        return res;
    }

} // namespace mln