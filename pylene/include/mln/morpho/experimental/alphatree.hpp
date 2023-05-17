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
    /// \brief Canvas for the edges in the alphatree.
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

    /// \brief Computes edges representing the input image
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
  } // namespace internal

  /// \brief the alphatree of an image
  ///
  /// \param I The input image
  /// \param N The neighborhood relation
  /// \param F Distance function
  template <class I, class N, class F>
  std::pair<component_tree<std::invoke_result_t<F, I, I>>, image_ch_value_t<I, int>> alphatree(I input, N nbh,
                                                                                               F distance)
  {

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());
    static_assert(::ranges::cpp20::invocable<F, image_value_t<I>, image_value_t<I>>);

    mln::image_ch_value_t<I, int> map = imchvalue<int>(input);
    auto                          dom = map.domain();
    int                           id  = 0;

    mln_foreach (auto p, dom)
    {
      map(p) = id;
      id++;
    }
    // mln::iota(map, 0);

    auto edges = internal::make_edges(std::move(input), std::move(nbh), map, std::move(distance));

    auto visitor =
        mln::morpho::experimental::canvas::kruskal_visitor_canonized<mln::image_ch_value_t<I, int>,
                                                                     std::invoke_result_t<F, I, I>>(id, std::move(map));
    mln::morpho::experimental::canvas::kruskal(visitor, std::move(edges));

    component_tree<std::invoke_result_t<F, I, I>> tree;
    tree.values = std::move(visitor.value);
    tree.parent = std::move(visitor.parent);

    return {std::move(tree), std::move(visitor.nodemap)};
  }
} // namespace mln::morpho::experimental