#pragma once

#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>
#include <mln/morpho/component_tree.hpp>
#include <mln/morpho/experimental/canvas/kruskal.hpp>

namespace mln::morpho::experimental
{
  namespace internal
  {
    template <typename W>
    class edge
    {
    public:
      int a;
      int b;
      W   w;

      edge(int a, int b, W w)
        : a(a)
        , b(b)
        , w(w)
      {
      }

      bool operator==(edge<W> o) const { return a == o.a && b == o.b && w == o.w; }
    };

    template <class I, class N, class F>
    std::vector<edge<std::invoke_result_t<F, I, I>>> make_edges(I input, N nbh, image_ch_value_t<I, int> map,
                                                                F distance)
    {
      auto edges = std::vector<edge<std::invoke_result_t<F, I, I>>>();
      auto dom   = input.domain();
      mln_foreach (auto p, dom)
      {
        for (auto n : nbh.after(p))
        {
          if (!dom.has(n))
            continue;
          edges.push_back({map(p), map(n), distance(input(p), input(n))});
        }
      }

      return edges;
    }

    template <class V, class E>
    void kruskal(V& visitor, std::vector<E>& adjList)
    {
      std::stable_sort(adjList.begin(), adjList.end(), [](E a, E b) -> bool { return a.w < b.w; });
      for (auto e : adjList)
      {
        int cx = visitor.on_find(e.a);
        int cy = visitor.on_find(e.b);
        if (cx != cy)
        {
          visitor.on_union(cx, cy, e);
        }
      }
    }

    int get_edge(int n, int nbNodes)
    {
      return n - nbNodes;
    }

    template <typename M>
    int weight_node(int n, int nbNodes, M MST)
    {
      int i = get_edge(n, nbNodes);
      return MST[i - 1].w;
    }

    template <typename M>
    std::vector<int> canonize_qbt(std::vector<int> parent, M MST, int size)
    {
      std::vector<int> res = std::vector<int>(size, 0);
      for (int n = 0; n < size; n++)
      {
        res[n] = parent[n];
      }
      for (int n = size - 2; n > size / 2; n--)
      {
        auto p = res[n];
        if (weight_node(p, size / 2, MST) == weight_node(n, size / 2, MST))
        {
          bool flag = false;
          for (int c = 0; c < size; c++)
          {
            if (parent[c] == n)
            {
              flag   = true;
              res[c] = p;
            }
          }
          if (flag)
            res[n] = n;
        }
      }
      return res;
    }

    template <typename W>
    component_tree<W> make_tree(std::vector<int> qt, std::vector<edge<W>> MST)
    {
      component_tree<W> tree;

      auto vals = std::vector<W>(qt.size(), 0);
      int  size = qt.size();
      for (int i = size / 2; i < size; i++)
      {
        if (qt[i] == i)
          vals[i] = 0;
        else
          vals[i] = weight_node(i, size / 2, MST);
      }
      tree.values = vals;
      tree.parent = std::move(qt);

      return tree;
    }

  } // namespace internal

  template <class I, class N, class F>
  std::pair<component_tree<std::invoke_result_t<F, I, I>>, image_ch_value_t<I, int>> alphatree(I input, N nbh,
                                                                                               F distance)
  {
    mln::image_ch_value_t<I, int> map = imchvalue<int>(input).set_init_value(-1);
    auto                          dom = map.domain();
    int                           id  = 0;
    mln_foreach (auto p, dom)
    {
      map(p) = id;
      id++;
    }
    //mln::iota

    auto edges = internal::make_edges(input, nbh, map, distance);

    auto visitor = mln::morpho::experimental::canvas::kruskal_visitor_base(id);
    auto MST     = internal::kruskal(visitor, edges);

    auto qt = internal::canonize_qbt(std::move(visitor.parent), MST, id * 2 - 1);

    auto tree = internal::make_tree(std::move(qt), std::move(MST));

    return {tree, map};
  }
} // namespace mln::morpho::experimental