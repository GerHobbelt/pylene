#pragma once

#include <ano/morpho/canvas/unionfind.hpp>

#include <memory>
#include <vector>

namespace ano::morpho::canvas
{
  namespace details
  {
    struct kruskal_default_visitor
    {
      /// \brief Visitor initialization
      void on_init(int) {}

      /// \brief Case one of the vertices has not been inserted
      /// \param p First vertex of the edge
      /// \param rp Root in the path compressed parent table of \p p
      /// \param q Second vertex of the edge
      /// \param rp Root in the path compressed parent table of \p q
      /// \param w Weight of the edge
      void on_union(ano::dontcare_t /*p*/, ano::dontcare_t /*rp*/, ano::dontcare_t /*q*/, ano::dontcare_t /*rq*/,
                    ano::dontcare_t /*w*/)
      {
      }

      /// \brief Case the edge is not added to the MST
      /// \param p First vertex of the edge
      /// \param rp Root in the path compressed parent table of \p p
      /// \param q Second vertex of the edge
      /// \param rp Root in the path compressed parent table of \p q
      /// \param w Weight of the edge
      void on_processing(ano::dontcare_t /*p*/, ano::dontcare_t /*rp*/, ano::dontcare_t /*q*/, ano::dontcare_t /*rq*/,
                         ano::dontcare_t /*w*/)
      {
      }

      /// \brief Final step
      void on_finish() {}
    };
  } // namespace details

  /// \brief Kruskal Minimum Spanning Tree canvas
  /// \param edges List of edges (should be sorted)
  /// \param size Size of the list of edges
  /// \param nvertices Number of vertices of the graph
  /// \param visitor Visitor for the pattern
  template <typename E, typename V>
  void kruskal(E edges, int size, int nvertices, V& visitor)
  {
    auto zpar = std::make_unique<int[]>(nvertices);
    std::iota(zpar.get(), zpar.get() + nvertices, 0);
    visitor.on_init(nvertices);

    for (int i = 0; i < size; i++)
    {
      auto [p, q, w] = edges[i];
      int rp         = impl::zfindroot(zpar.get(), p);
      int rq         = impl::zfindroot(zpar.get(), q);
      if (rp != rq)
      {
        zpar[rp] = rq;
        visitor.on_union(p, rp, q, rq, w);
      }
      else
        visitor.on_processing(p, rp, q, rq, w);
    }

    visitor.on_finish();
  }
} // namespace ano::morpho::canvas