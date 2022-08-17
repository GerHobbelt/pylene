#include <ano/morpho/private/trees_fusion.hpp>

#include <numeric>
#include <stack>

namespace ano::morpho::details
{
  namespace
  {
    std::vector<int> smallest_enclosing_shape(const component_tree<>& ti, image2d<int> ni, const component_tree<>& tj,
                                              image2d<int> nj, const std::vector<int>& depth)
    {
      const auto lca = [&depth, &tj](int a, int b) {
        if (a < 0)
          return b;
        if (b < 0)
          return a;
        while (depth[a] < depth[b])
          b = tj.parent[b];
        while (depth[b] < depth[a])
          a = tj.parent[a];
        while (a != b)
        {
          a = tj.parent[a];
          b = tj.parent[b];
        }
        return a;
      };

      std::vector<int> res(ti.parent.size(), -1);
      ano_foreach (auto p, ni.domain())
        res[ni(p)] = lca(res[ni(p)], nj(p));

      for (int n = (int)ti.parent.size() - 1; n > 0; n--)
        res[ti.parent[n]] = lca(res[ti.parent[n]], res[n]);

      return res;
    }

    std::vector<int> topo_sort(const std::vector<std::set<int>>& g)
    {
      static constexpr int UNMARKED  = 0;
      static constexpr int TEMPORARY = 1;
      static constexpr int PERMANENT = 2;

      std::vector<int> res(g.size());
      int              cur = static_cast<int>(g.size()) - 1;

      std::vector<int> visited(g.size(), UNMARKED);
      std::stack<int>  st;

      for (int i = 0; i < (int)g.size(); i++)
      {
        st.push(i);

        while (!st.empty())
        {
          const auto v = st.top();
          st.pop();
          if (visited[v] == PERMANENT)
            continue;
          else if (visited[v] == TEMPORARY)
          {
            res[cur--] = v;
            visited[v] = PERMANENT;
          }
          else
          {
            visited[v] = TEMPORARY;
            st.push(v);
            for (int e : g[v])
              st.push(e);
          }
        }
      }

      return res;
    }
  } // namespace

  std::pair<std::vector<std::set<int>>, std::vector<std::vector<int>>>
  compute_inclusion_graph(component_tree<>* trees, image2d<int>* nodemaps, std::vector<int>* depths, int ntrees)
  {
    std::vector<std::vector<int>> tree_to_graph(ntrees); // Link tree node -> graph node
    std::vector<std::set<int>>    graph(1); // The graph (the container of out vertices is a set since there can only be
                                            // one edge between two nodes, with ensure faster result for removing)

    const auto add_vertex = [&graph]() -> int {
      int n = static_cast<int>(graph.size());
      graph.push_back(std::set<int>());
      return n;
    };

    // Computing SES (the SES of one node in the same tree is itself)
    std::vector<std::vector<int>> ses(ntrees * ntrees);
    for (int i = 0; i < ntrees; i++)
    {
      for (int j = 0; j < ntrees; j++)
      {
        if (i != j)
          ses[i * ntrees + j] = smallest_enclosing_shape(trees[i], nodemaps[i], trees[j], nodemaps[j], depths[j]);
        else
        {
          ses[i * 3 + j].resize(trees[i].parent.size());
          std::iota(ses[i * 3 + j].begin(), ses[i * 3 + j].end(), 0);
        }
      }
    }

    // Informations on each node of the graph related to each tree
    std::vector<std::vector<int>> graph_to_tree(ntrees, {0});
    std::vector<std::vector<int>> node_depths(ntrees, {0});

    // Initialize tree to graph
    for (int i = 0; i < ntrees; i++)
    {
      tree_to_graph[i].resize(trees[i].parent.size());
      tree_to_graph[i][0] = 0;
    }

    // All the node of the first tree are added, so we prevent one jump
    for (int n = 1; n < (int)trees[0].parent.size(); n++)
    {
      auto new_vertex     = add_vertex();
      tree_to_graph[0][n] = new_vertex;
      for (int j = 0; j < 3; j++)
      {
        node_depths[j].push_back(depths[j][ses[j][n]]);
        graph_to_tree[j].push_back(j == 0 ? n : -1);
      }
    }

    // Adding the other nodes, testing if they do not still exists
    for (int i = 1; i < ntrees; i++)
    {
      const auto& ti = trees[i];
      for (int n = 1; n < (int)ti.parent.size(); n++)
      {
        int j = 0;
        for (; j < i; j++)
        {
          const auto tmp = ses[i * ntrees + j][n];
          if (ses[j * ntrees + i][tmp] == n)
          {
            const auto v        = tree_to_graph[j][tmp];
            tree_to_graph[i][n] = v;
            graph_to_tree[i][v] = n;
            break;
          }
        }
        if (i == j)
        {
          int new_vertex      = add_vertex();
          tree_to_graph[i][n] = new_vertex;
          for (int j = 0; j < ntrees; j++)
          {
            node_depths[j].push_back(depths[j][ses[i * ntrees + j][n]]);
            graph_to_tree[j].push_back(j == 0 ? n : -1);
          }
        }
      }
    }

    // Adding the edges
    for (int i = 0; i < ntrees; i++)
    {
      const auto& ti = trees[i];
      for (int n = 1; n < (int)ti.parent.size(); n++)
      {
        graph[tree_to_graph[i][n]].insert(tree_to_graph[i][ti.parent[n]]);
        for (int j = 0; j < ntrees; j++)
        {
          if (i != j && ses[j * ntrees + i][ses[i * ntrees + j][n]] != n)
            graph[tree_to_graph[i][n]].insert(tree_to_graph[j][ses[i * ntrees + j][n]]);
        }
      }
    }

    // Reduction step
    const auto comp = [&node_depths, &ntrees](int a, int b) {
      int i = 0;
      while (i < ntrees && node_depths[i][a] > node_depths[i][b])
        i++;
      return i == ntrees;
    };
    for (int v = 0; v < (int)graph.size(); v++)
    {
      std::set<int> to_remove;
      for (int d1 : graph[v])
      {
        for (int d2 : graph[v])
        {
          if (d1 != d2 && comp(d1, d2))
            to_remove.insert(d2);
        }
      }
      for (int e : to_remove)
        graph[v].erase(e);
    }

    return {std::move(graph), std::move(tree_to_graph)};
  }

  image2d<std::uint16_t> compute_depth_map(const std::vector<std::set<int>>&    graph,
                                           const std::vector<std::vector<int>>& tree_to_graph, image2d<int>* nodemaps)
  {
    image2d<std::uint16_t> res(nodemaps[0].domain());
    std::vector<int>       graph_depth(graph.size(), -1);
    graph_depth[0] = 0;

    /// FIXME: Optimization removing the transpose (modify topo_sort)
    std::vector<std::set<int>> gT(graph.size());
    for (int v = 0; v < (int)graph.size(); v++)
    {
      for (int o : graph[v])
        gT[o].insert(v);
    }

    int max_depth = 0;
    for (int v : topo_sort(gT))
    {
      for (int e : gT[v])
      {
        graph_depth[e] = std::max(graph_depth[e], graph_depth[v] + 1);
        max_depth      = std::max(max_depth, graph_depth[e]);
      }
    }

    if (max_depth >= (1 << 16))
      throw std::runtime_error("The input graph is too deep");

    ano_foreach (auto p, nodemaps[0].domain())
    {
      int d = graph_depth[tree_to_graph[0][nodemaps[0](p)]];
      for (int i = 1; i < (int)tree_to_graph.size(); i++)
        d = std::max(d, graph_depth[tree_to_graph[i][nodemaps[i](p)]]);
      res(p) = static_cast<std::uint16_t>(d);
    }
    return res;
  }
} // namespace ano::morpho::details