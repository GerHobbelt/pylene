#pragma once

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/range/foreach.hpp>

#include <mln/core/utils/dontcare.hpp>
#include <vector>

namespace mln::morpho::experimental::canvas
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
  } // namespace internal
  template <class I>
  class kruskal_visitor_base
  {
  public:
    kruskal_visitor_base(int size, I nd)
      : parent(2 * size - 1)
      , m_root(size)
      , m_parent_c(size, -1)
      , m_rank(size, 0)
      , m_size(size)
    {
      nodemap = nd;
      for (int i = 0; i < m_size; i++)
      {
        m_root[i] = i;
        parent[i] = i;
      }
    }

    void on_start() {}

    int on_find(int e)
    {
      int r = e;
      while (m_parent_c[r] >= 0)
        r = m_parent_c[r];
      while (m_parent_c[e] >= 0)
      {
        auto tmp        = e;
        e               = m_parent_c[e];
        m_parent_c[tmp] = r;
      }
      return r;
    }

    void on_union(int cx, int cy, mln::dontcare_t e)
    {
      (void)e;
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
    }

    void on_finish() { make_tree(); }

  protected:
    void make_tree()
    {
      std::vector<int> res = {};
      int              nb  = 0;
      for (auto i = m_size - 1; i > m_size / 2; --i)
      {
        if (parent[i] == i)
          continue;
        if (nb == 0)
        {
          res.push_back(0);
        }
        else
        {
          res.push_back(parent[parent[i]]);
        }
        parent[i] = nb;
        nb++;
      }
      for (auto i = 0; i < m_size / 2 + 1; i++)
      {
        res.push_back(parent[parent[i]]);
      }

      int  i   = 0;
      auto dom = nodemap.domain();

      mln_foreach (auto p, dom)
      {
        nodemap(p) = parent[parent[i]];
        i++;
      }

      parent = std::move(res);
    }

    void make_set(int q)
    {
      parent[q] = -1;
      m_size += 1;
    }

  public:
    std::vector<int> parent;
    I                nodemap;

  protected:
    std::vector<int> m_root;
    std::vector<int> m_parent_c;
    std::vector<int> m_rank;
    int              m_size;
  };


  template <class I, class W>
  class kruskal_visitor_mst : public kruskal_visitor_base<I>
  {
  public:
    kruskal_visitor_mst(int size, I nd)
      : kruskal_visitor_base<I>(size, nd)
      , mst()
    {
    }

    void on_union(int cx, int cy, internal::edge<W> e)
    {
      int tu           = this->m_root[cx];
      int tv           = this->m_root[cy];
      this->parent[tu] = this->m_size;
      this->parent[tv] = this->m_size;

      if (this->m_rank[cx] > this->m_rank[cy])
        std::swap(cx, cy);
      if (this->m_rank[cx] == this->m_rank[cy])
        this->m_rank[cy] += 1;
      this->m_parent_c[cx] = cy;
      int c                = cy;

      this->m_root[c] = this->m_size;
      this->make_set(this->m_size);
      mst.push_back(e);
    }

  public:
    std::vector<internal::edge<W>> mst;
  };

  template <class I, class W>
  class kruskal_visitor_values : public kruskal_visitor_base<I>
  {
  public:
    kruskal_visitor_values(int size, I nd)
      : kruskal_visitor_base<I>(size, nd)
      , value(size, 0)
    {
    }

    void on_finish()
    {
      this->make_tree();
      for (int i = 1; i < this->m_size / 2; i++)
      {
        if (this->parent[i] == i)
          value[i] = 0;
      }
    }

    void on_union(int cx, int cy, auto e)
    {
      int tu           = this->m_root[cx];
      int tv           = this->m_root[cy];
      this->parent[tu] = this->m_size;
      this->parent[tv] = this->m_size;

      if (this->m_rank[cx] > this->m_rank[cy])
        std::swap(cx, cy);
      if (this->m_rank[cx] == this->m_rank[cy])
        this->m_rank[cy] += 1;
      this->m_parent_c[cx] = cy;
      int c                = cy;

      this->m_root[c] = this->m_size;
      this->make_set(this->m_size);
      value.insert(value.begin(), e.w);
    }

  public:
    std::vector<W> value;
  };

  template <class I, class W>
  class kruskal_visitor_canonized : public kruskal_visitor_values<I, W>
  {
  public:
    kruskal_visitor_canonized(int size, I nd)
      : kruskal_visitor_values<I, W>(size, nd)
    {
    }
    void on_finish()
    {
      for (int i = 1; i < this->m_size / 2; i++)
      {
        if (this->parent[i] == i)
          this->value[i] = 0;
      }
      std::vector<int> canonized = std::vector<int>(this->m_size);
      for (int n = 0; n < this->m_size; n++)
      {
        canonized[n] = this->parent[n];
      }
      for (int n = this->m_size - 2; n > this->m_size / 2; n--)
      {
        auto p = canonized[n];
        if (this->value[this->m_size - 1 - p] == this->value[this->m_size - 1 - n])
        {
          for (int c = 0; c < this->m_size; c++)
          {
            if (this->parent[c] == n)
            {
              canonized[c] = p;
            }
          }
          canonized[n] = n;
        }
      }
      this->parent = canonized;
      this->make_tree();
      int i = this->parent.size();
      mln_foreach (auto p, this->nodemap.domain())
      {
        if (this->value[this->nodemap(p)] != 0)
        {
          this->parent.push_back(this->nodemap(p));
          this->nodemap(p) = i;
          i++;
          this->value.push_back(0);
        }
      }
    }

  protected:
    void make_tree()
    {
      std::vector<int> npar = {};
      std::vector<W>   nval = {};
      int              nb   = 0;
      for (auto i = this->m_size - 1; i > this->m_size / 2; --i)
      {
        if (this->parent[i] == i)
          continue;
        if (nb == 0)
        {
          npar.push_back(0);
        }
        else
        {
          npar.push_back(this->parent[this->parent[i]]);
        }
        nval.push_back(this->value[this->m_size - i - 1]);
        this->parent[i] = nb;
        nb++;
      }
      int  i   = 0;
      auto dom = this->nodemap.domain();
      mln_foreach (auto p, dom)
      {
        this->nodemap(p) = this->parent[this->parent[i]];
        i++;
      }

      this->parent = std::move(npar);
      this->value  = std::move(nval);
    }
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