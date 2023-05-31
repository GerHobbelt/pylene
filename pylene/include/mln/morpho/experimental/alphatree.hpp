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

    void iota(mln::ndbuffer_image& a)
    {
      int i = 0;
      for (int w = 0; w < a.size(3); ++w)
        for (int z = 0; z < a.depth(); ++z)
          for (int y = 0; y < a.height(); ++y)
          {
            std::uint32_t* lineptr =
                reinterpret_cast<std::uint32_t*>(a.buffer()) + y * a.stride(1) + z * a.stride(2) + w * a.stride(3);
            int width = a.width();
            for (int x = 0; x < width; ++x)
              lineptr[x] = i++;
          }
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
    static_assert(std::is_invocable_v<F, image_value_t<I>, image_value_t<I>>);

    mln::image_ch_value_t<I, int> map = imchvalue<int>(input);
    mln::iota(map, 0);

    auto edges = internal::make_edges(std::move(input), nbh, map, std::move(distance));

    auto visitor = mln::morpho::experimental::canvas::kruskal_visitor_canonized<mln::image_ch_value_t<I, int>,
                                                                                std::invoke_result_t<F, I, I>>(
        map.domain().size(), std::move(map));
    mln::morpho::experimental::canvas::kruskal(visitor, std::move(edges));

    component_tree<std::invoke_result_t<F, I, I>> tree;
    tree.values = std::move(visitor.value);
    tree.parent = std::move(visitor.parent);

    return {std::move(tree), std::move(visitor.nodemap)};
  }

  template <class N, class F>
  auto __alphatree(std::any input, N nbh, F distance)
  {
    (void)nbh;
    using __type_w        = std::invoke_result_t<F, std::any, std::any>;
    mln::ndbuffer_image a = std::any_cast<mln::ndbuffer_image>(input);

    auto map = mln::image2d<int>(a.domain());
    internal::iota(map);

    auto edges = std::vector<internal::edge<std::invoke_result_t<F, std::any, std::any>>>();

    auto dom = a.domain();

    for (int x = 0; x < a.width(); x++)
    {
      for (int y = 0; y < a.height(); y++)
      {
        for (auto&& qix : nbh(mln::point2d{x, y}))
        {
          if (!dom.has(qix))
            continue;
          edges.push_back({map(mln::point2d{x, y}), map(qix),
                           distance(*static_cast<const __type_w*>(a({x, y})), *static_cast<const __type_w*>(a(qix)))});
        }
      }
    }

    auto visitor = mln::morpho::experimental::canvas::kruskal_visitor_canonized<mln::image2d<int>, __type_w>(
        map.domain().size(), std::move(map));
    mln::morpho::experimental::canvas::kruskal(visitor, std::move(edges));

    component_tree<__type_w> tree;
    tree.values = visitor.value;
    tree.parent = std::move(visitor.parent);

    return std::pair<component_tree<std::invoke_result_t<F, std::any, std::any>>,
                     image_ch_value_t<mln::image2d<int>, int>>(tree, std::move(visitor.nodemap));
  }
} // namespace mln::morpho::experimental