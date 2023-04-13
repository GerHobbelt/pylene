#pragma once

#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/image/ndimage.hpp>

#include <mln/core/utils/dontcare.hpp>
#include <vector>

namespace mln::morpho::experimental::canvas
{
  class kruskal_visitor_base
  {
  public:
    kruskal_visitor_base(int size);

    void on_start();
    int  on_find(int e);
    void on_union(int cx, int cy, mln::dontcare_t e);
    void on_finish();

  protected:
    void make_set(int q);

  public:
    std::vector<int> parent;

  protected:
    std::vector<int> m_root;
    std::vector<int> m_parent_c;
    std::vector<int> m_rank;
    int              m_size;
  };

  template <class E>
  class kruskal_visitor_mst : public kruskal_visitor_base
  {
  public:
    kruskal_visitor_mst(int size)
      : kruskal_visitor_base(size)
      , mst()
    {
    }

    void on_union(int cx, int cy, E e)
    {
      int tu     = m_root[cx];
      int tv     = m_root[cy];
      parent[tu] = m_size;
      parent[tv] = m_size;

      if (m_rank[cx] > m_rank[cy])
        std::swap(cx, cy);
      if (m_rank[cx] == m_rank[cy])
        m_rank[cy] += 1;
      m_parent_c[cx] = cy;
      int c          = cy;

      m_root[c] = m_size;
      make_set(m_size);
      mst.push_back(e);
    }

  public:
    std::vector<E> mst;
  };

  template <class E>
  class kruskal_visitor_values : public kruskal_visitor_mst<E>
  {
  public:
    kruskal_visitor_values(int size)
      : kruskal_visitor_mst<E>(size)
      , values(size * 2 - 1, 0)
    {
    }

    void on_finish()
    {
      for (int i = this->m_size / 2; i < this->m_size; i++)
      {
        if (this->parent[i] == i)
          values[i] = 0;
        else
          values[i] = this->weight_node(i);
      }
    }

    int get_edge(int n, int nbNodes) { return n - nbNodes; }

    int weight_node(int n)
    {
      int i = get_edge(n, this->m_size / 2);
      return mst[i - 1].w;
    }

  protected:
    using kruskal_visitor_mst<E>::mst;

  public:
    std::vector<int> values;
  };

  class kruskal_visitor_nodemap : public kruskal_visitor_base
  {
  public:
    kruskal_visitor_nodemap(int size)
      : kruskal_visitor_base(size)
    {
      mln::iota(nodemap, 0);
    }

  public:
    mln::image1d<std::uint8_t> nodemap;
  };


  template <class V, class E>
  void kruskal(V& visitor, std::vector<E>& adjList)
  {
    visitor.on_start();
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
    visitor.on_finish();
  }
} // namespace mln::morpho::experimental::canvas