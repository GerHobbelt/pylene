#pragma once

#include <map>

#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>

#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/functional_ops.hpp>
#include <mln/morpho/canvas/unionfind.hpp>
#include <mln/morpho/component_tree.hpp>

#include <mln/morpho/private/directional_hqueue.hpp>

#include <algorithm>
#include <type_traits>

namespace mln::morpho::experimental
{
  namespace internal
  {
    template <typename W>
    class edge
    {
      int a_;
      int b_;
      W   w_;

    public:
      edge(int a, int b, W w)
        : a_(a)
        , b_(b)
        , w_(w)
      {
      }
      int first() { return a_; }
      int second() { return b_; }
      W   weight() { return w_; }

      bool operator==(edge<W> o) const { return a_ == o.first() && b_ == o.second() && w_ == o.weight(); }
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

      void MakeSet(int q)
      {
        parent[q] = -1;
        size += 1;
      }

      int FindCanonical(int q)
      {
        while (parent[q] >= 0)
          q = parent[q];
        return q;
      }

      void Union(int cx, int cy)
      {
        parent[cx] = size;
        parent[cy] = size;
        MakeSet(size);
      }
    };

    class QT
    {
    public:
      std::vector<int> parent;
      std::vector<int> rank;
      int size;

      QT(int n)
        : parent(std::vector<int>(n, -1))
        , rank(std::vector<int>(n, 0))
        , size(0)
      {
      }

      void MakeSet(int q)
      {
        (void) q;
        parent[size] = -1;
        rank[size]   = 0;
        size += 1;
      }

      int FindCanonical(int q)
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

      int Union(int cx, int cy)
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
      QBT qbt;
      QT qt;
      int size;

      QEBT(int n)
        : root(std::vector<int>(n, 0))
        , qbt(QBT(n))
        , qt(QT(n))
        , size(0)
      {
      }

      void MakeSet(int q)
      {
        root[q] = q;
        qbt.MakeSet(q);
        qt.MakeSet(q);
      }

      int FindCanonical(int q)
      {
        return qt.FindCanonical(q);
      }

      void Union(int cx, int cy)
      {
        int tu = root[cx];
        int tv = root[cy];
        qbt.parent[tu] = qbt.size;
        qbt.parent[tv] = qbt.size;
        int c = qt.Union(cx, cy);
        root[c] = qbt.size;
        qbt.MakeSet(qbt.size);
      }
    };


    template <class I, class N, class P, class W>
    void MakeAdjList(I input, N nbh, std::map<P, int> map, std::vector<edge<W>>& adjList)
    {
      auto dom = input.domain();
      mln_foreach (auto p, dom)
      {
        for (auto n : nbh.after(p))
        {
          if (!dom.has(p) || !dom.has(n))
            continue;
          W weight;
          if (input(p) > input(n))
            weight = input(p) - input(n);
          else
            weight = input(n) - input(p);
          adjList.push_back({map[p], map[n], weight});
        }
      }
    }

    template <class Q, class E>
    std::vector<E> Kruskal(Q& q, std::vector<E>& adjList, int nbNodes)
    {
      int            c   = 0;
      std::vector<E> MST = {};
      std::stable_sort(adjList.begin(), adjList.end(), [](E a, E b) -> bool { return a.weight() < b.weight(); });
      for (int i = 0; i < nbNodes; i++)
        q.MakeSet(i);
      for (auto e : adjList)
      {
        auto cx = q.FindCanonical(e.first());
        auto cy = q.FindCanonical(e.second());
        if (cx != cy)
        {
          q.Union(cx, cy);
          MST.push_back(e);
          c += 1;
        }
      }
      return MST;
    }

    int getEdge(int n, int nbNodes)
    {
      return n - nbNodes;
    }

    template <typename M>
    int weightNode(int n, int nbNodes, M MST)
    {
      int i = getEdge(n, nbNodes);
      return MST[i - 1].weight();
    }

    template <typename M>
    std::vector<int> CanonizeQbt(QBT qbt, M MST)
    {
      std::vector<int> res = std::vector<int>(qbt.size, 0);
      for (int n = 0; n < qbt.size; n++)
      {
        res[n] = qbt.parent[n];
      }
      for (int n = qbt.size - 3; n > qbt.size / 2; n--)
      {
        auto p = res[n];
        if (weightNode(p, qbt.size / 2, MST) == weightNode(n, qbt.size / 2, MST))
        {
          bool flag = false;
          for (int c = 0; c < qbt.size / 2; c++)
          {
            if (qbt.parent[c] == n)
            {
              flag = true;
              res[c] = p;
            }
          }
          if (flag)
            res[n] = n;
        }
      }
      return res;
    }

  } // namespace internal
} // namespace mln::morpho::experimental