#pragma once


#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>

#include <mln/core/functional_ops.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/morpho/experimental/component_tree.hpp>
#include <mln/morpho/experimental/canvas/unionfind.hpp>

#include <type_traits>
#include <algorithm>

#include <range/v3/functional/concepts.hpp>


namespace mln::morpho::experimental
{

  /// Compute the alphatree of an image
  ///
  /// \param input The input image
  /// \param neighborhood The neighborhood relation
  /// \param distance Distance function
  template <class I, class N, class F = mln::functional::l2dist_t<>>
  std::pair<component_tree<std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>, image_ch_value_t<I, int>> //
  alphatree(I input, N nbh, F distance = F{});


  /******************************************/
  /****          Implementation          ****/
  /******************************************/



  namespace internal
  {

    template <class P, class W>
    struct alphatree_edge_t
    {
      P            a;
      P            b;
      W            w;
    };


    // Compute a node_id for each flat zone
    template <class I, class J>
    [[gnu::noinline]]
    std::size_t alphatree_create_nodemap(I node_map, J zpar)
    {
      std::size_t node_count = 0;
      mln_foreach_new(auto px, node_map.new_pixels())
      {
        auto p  = px.point();
        auto rp = canvas::impl::zfindroot(zpar, p);
        if (node_map(rp) < 0)
          node_map(rp) = static_cast<int>(node_count++);
        node_map(p) = node_map(rp);
      }
      return node_count;
    }

    // Compute flat zone of the image using union-find structures
    // It returns the number of edges processed
    template <class E, class J>
    std::size_t alphatree_compute_flatzones(const E* edges, std::size_t n, J zpar)
    {
      canvas::impl::union_find_init_par(zpar);

      std::size_t i;
      for (i = 0; i < n && edges[i].w == 0; ++i)
      {
        auto p = edges[i].a;
        auto q = edges[i].b;
        auto rp = canvas::impl::zfindroot(zpar, p);
        auto rq = canvas::impl::zfindroot(zpar, q);
        if (rp != rq) // Merge set
          zpar(rq) = rp;
      }
      return i;
    }

    // Inverse the node array, updating the parent relation
    template <class W>
    [[gnu::noinline]] void alphatree_reorder_nodes(int* parent, W* levels, std::size_t n)
    {
      std::size_t i = 0;
      std::size_t j = n - 1;

      while (i < j)
      {
        std::swap(levels[i], levels[j]);
        std::swap(parent[i], parent[j]);
        parent[i] = static_cast<int>(n) - parent[i] - 1;
        parent[j] = static_cast<int>(n) - parent[j] - 1;
        i++;
        j--;
      }
      if (i == j)
        parent[i] = static_cast<int>(n) - parent[i] - 1;
    }


    template <class I, class N, class F, class E>
    void alphatree_compute_edges(I input, N nbh, F distance, std::vector<E>& edges)
    {
      auto dom = input.domain();
      mln_foreach_new (auto p, dom)
      {
        for (auto n : nbh.after(p))
          if (dom.has(n))
            edges.push_back({p, n, distance(input(p), input(n))});
      }
    }

    //
    //
    template <class E, class I, class W>
    std::size_t alphatree_compute_hierarchy(const E* edges, std::size_t n, I node_map, //
                                            std::size_t       node_count,              //
                                            std::vector<int>& par,                     //
                                            std::vector<W>&   levels)
    {
      static_assert(mln::is_a<I, mln::details::Image>());



      std::vector<int> zpar(node_count);   // Parent of ufind structure
      std::vector<int> links(node_count);  // Node index in the alpha tree

      std::iota(std::begin(zpar), std::end(zpar), 0);
      std::iota(std::begin(links), std::end(links), 0);

      for (std::size_t i = 0; i < n; ++i)
      {
        auto [p, q, w] = edges[i];
        auto rp        = canvas::impl::zfindroot(zpar.data(), node_map(p));
        auto rq        = canvas::impl::zfindroot(zpar.data(), node_map(q));

        if (rp != rq)
        {
          // Merge set

          assert(levels[rp] <= w && levels[rq] <= w);
          zpar[rq] = rp;

          // Do we need to create a new node
          int rp_root = links[rp];
          int rq_root = links[rq];
          int new_root_id;
          if (levels[rq_root] == w)
          {
            new_root_id = rq_root;
          }
          else if (levels[rp_root] == w)
          {
            new_root_id = rp_root;
          }
          else
          {
            // Create a new_node
            new_root_id = static_cast<int>(node_count++);
            par.push_back(new_root_id);
            levels.push_back(w);
          }
          par[rp_root] = new_root_id;
          par[rq_root] = new_root_id;
          links[rp]    = new_root_id;
        }
      }
      return node_count;
    }
  } // namespace internal

  template <class I, class N, class F>
  std::pair<component_tree<std::invoke_result_t<F, image_value_t<I>, image_value_t<I>>>, image_ch_value_t<I, int>> //
  alphatree(I input, N nbh, F distance)
  {
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());
    static_assert(::ranges::cpp20::invocable<F, image_value_t<I>, image_value_t<I>>);

    using V = image_value_t<I>;
    using P = image_point_t<I>;
    using W = std::invoke_result_t<F, V, V>;

    static_assert(::concepts::totally_ordered<W>);

    using edge_t = internal::alphatree_edge_t<P, W>;



    // 1. Get the list of edges
    std::vector<edge_t> edges;
    internal::alphatree_compute_edges(std::move(input), std::move(nbh), std::move(distance), edges);

    // Sort the edges by increasing weights
    std::sort(std::begin(edges), std::end(edges), [](const auto& a, const auto& b) { return a.w < b.w; });

    std::size_t              edges_null_count;
    std::size_t              flatzones_count;
    image_ch_value_t<I, int> node_map = imchvalue<int>(input).set_init_value(-1);
    {
      image_ch_value_t<I, P>   zpar     = imchvalue<P>(input);
      // 2. Compute flat zone of the image
      edges_null_count = internal::alphatree_compute_flatzones(edges.data(), edges.size(), zpar);

      // 3. Compute a node_id for each flat zone
      flatzones_count = internal::alphatree_create_nodemap(node_map, zpar);
    }

    std::size_t node_count = flatzones_count;

    std::vector<int> par(node_count);
    std::vector<int> levels(node_count, 0);
    // 4. Compute the hierarchy
    {
      std::iota(std::begin(par), std::end(par), 0);
      node_count = internal::alphatree_compute_hierarchy(edges.data() + edges_null_count, edges.size() - edges_null_count,
                                                         node_map, node_count, par, levels);
    }

    // 5. Parent / levels are ordered from leaves to root, we need to reverse
    internal::alphatree_reorder_nodes(par.data(), levels.data(), node_count);

    // 6. Update the node_map
    mln::for_each(node_map, [node_count](int& id) { id = static_cast<int>(node_count) - id - 1; });

    component_tree<W> t;
    t.parent = std::move(par);
    t.values = std::move(levels);

    return {std::move(t), std::move(node_map)};
  }

}
