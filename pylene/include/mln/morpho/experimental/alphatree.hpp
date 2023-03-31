#pragma once

#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>
#include <mln/morpho/component_tree.hpp>

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

    class QBT
    {
    public:
      std::vector<int> parent;
      int              size;

      QBT(int n)
        : parent(std::vector<int>(2 * n - 1, -1))
        , size(0)
      {
      }

      void make_set(int q)
      {
        parent[q] = -1;
        size += 1;
      }

      int find_canonical(int q)
      {
        while (parent[q] >= 0)
          q = parent[q];
        return q;
      }

      void union_q(int cx, int cy)
      {
        parent[cx] = size;
        parent[cy] = size;
        make_set(size);
      }
    };

    class QT
    {
    public:
      std::vector<int> parent;
      std::vector<int> rank;
      int              size;

      QT(int n)
        : parent(std::vector<int>(n, -1))
        , rank(std::vector<int>(n, 0))
        , size(0)
      {
      }

      void make_set(int q)
      {
        (void)q;
        parent[size] = -1;
        rank[size]   = 0;
        size += 1;
      }

      int find_canonical(int q)
      {
        int r = q;
        while (parent[r] >= 0)
          r = parent[r];
        while (parent[q] >= 0)
        {
          auto tmp    = q;
          q           = parent[q];
          parent[tmp] = r;
        }
        return r;
      }

      int union_q(int cx, int cy)
      {
        if (rank[cx] > rank[cy])
          std::swap(cx, cy);
        if (rank[cx] == rank[cy])
          rank[cy] += 1;
        parent[cx] = cy;
        return cy;
      }
    };

    class QEBT
    {
    public:
      std::vector<int> root;
      QBT              qbt;
      QT               qt;
      int              size;

      QEBT(int n)
        : root(std::vector<int>(n, 0))
        , qbt(QBT(n))
        , qt(QT(n))
        , size(0)
      {
      }

      void make_set(int q)
      {
        root[q] = q;
        qbt.make_set(q);
        qt.make_set(q);
      }

      int find_canonical(int q) { return qt.find_canonical(q); }

      void union_q(int cx, int cy)
      {
        int tu         = root[cx];
        int tv         = root[cy];
        qbt.parent[tu] = qbt.size;
        qbt.parent[tv] = qbt.size;
        int c          = qt.union_q(cx, cy);
        root[c]        = qbt.size;
        qbt.make_set(qbt.size);
      }
    };


    template <class I, class N, class F>
    std::vector<edge<std::invoke_result_t<F, I, I>>> make_edges(I input, N nbh, image_ch_value_t<I, int> map,
                                                                F distance)
    {
      std::vector<edge<std::invoke_result_t<F, I, I>>> edges = {};
      auto                                             dom   = input.domain();
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

    template <class Q, class E>
    std::vector<E> kruskal(Q& q, std::vector<E>& adjList, int nbNodes)
    {
      int            c   = 0;
      std::vector<E> MST = {};
      std::stable_sort(adjList.begin(), adjList.end(), [](E a, E b) -> bool { return a.w < b.w; });
      for (int i = 0; i < nbNodes; i++)
        q.make_set(i);
      for (auto e : adjList)
      {
        auto cx = q.find_canonical(e.a);
        auto cy = q.find_canonical(e.b);
        if (cx != cy)
        {
          q.union_q(cx, cy);
          MST.push_back(e);
          c += 1;
        }
      }
      return MST;
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
    std::vector<int> canonize_qbt(QBT qbt, M MST)
    {
      std::vector<int> res = std::vector<int>(qbt.size, 0);
      for (int n = 0; n < qbt.size; n++)
      {
        res[n] = qbt.parent[n];
      }
      for (int n = qbt.size - 2; n > qbt.size / 2; n--)
      {
        auto p = res[n];
        if (weight_node(p, qbt.size / 2, MST) == weight_node(n, qbt.size / 2, MST))
        {
          bool flag = false;
          for (int c = 0; c < qbt.size; c++)
          {
            if (qbt.parent[c] == n)
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

    std::vector<internal::edge<std::invoke_result_t<F, I, I>>> edges = internal::make_edges(input, nbh, map, distance);

    mln::morpho::experimental::internal::QEBT                  q = {id};
    std::vector<internal::edge<std::invoke_result_t<F, I, I>>> MST =
        mln::morpho::experimental::internal::kruskal(q, edges, id);

    auto qt = mln::morpho::experimental::internal::canonize_qbt(std::move(q.qbt), MST);

    auto tree = mln::morpho::experimental::internal::make_tree(std::move(qt), std::move(MST));

    return {tree, map};
  }
} // namespace mln::morpho::experimental