#pragma once


#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>

#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/functional_ops.hpp>
#include <mln/morpho/canvas/unionfind.hpp>
#include <mln/morpho/component_tree.hpp>

#include <mln/morpho/private/directional_hqueue.hpp>

#include <algorithm>
#include <type_traits>

#include <range/v3/functional/concepts.hpp>


namespace mln::morpho
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
    template <typename I, typename N, typename W>
    class alphatree_edges;

    template <typename I, typename N, typename W>
    requires(std::is_integral_v<W>&& std::is_unsigned_v<W> && sizeof(W) <= 2) class alphatree_edges<I, N, W>
    {
      static_assert(is_a_v<I, mln::details::Image>);
      using P = image_point_t<I>;

    public:
      alphatree_edges(){};
      void                push(std::size_t dir, W w, P p) { m_cont.insert(dir, w, p); }
      std::tuple<P, P, W> pop() { return m_cont.pop(); }
      W                   top() const { return m_cont.current_level(); }
      bool                empty() const { return m_cont.empty(); }
      void                on_finish_insert() {}

    private:
      details::directional_hqueue<I, N, W> m_cont;
    };

    template <typename P, typename W>
    struct edge_t
    {
      P p;
      P q;
      W w;
    };

    template <typename I, typename N, typename W>
    class alphatree_edges
    {
      static_assert(is_a_v<I, mln::details::Image>);
      using P = image_point_t<I>;

    public:
      alphatree_edges()
        : m_current(0)
      {
      }
      void                push(std::size_t dir, W w, P p) { m_cont.push_back({p, p + cn.after_offsets()[dir], w}); }
      std::tuple<P, P, W> pop()
      {
        assert(m_current < m_cont.size());
        const auto e = m_cont[m_current++];
        return {std::move(e.p), std::move(e.q), std::move(e.w)};
      }

      W top() const
      {
        assert(m_current < m_cont.size());
        return m_cont[m_current].w;
      }
      bool empty() const { return m_cont.size() == m_current; }
      void on_finish_insert()
      {
        std::sort(m_cont.begin(), m_cont.end(), [](const edge_t<P, W>& a, const edge_t<P, W>& b) { return a.w < b.w; });
      }

    private:
      static constexpr auto     cn = N();
      std::vector<edge_t<P, W>> m_cont;
      std::size_t               m_current;
    };


    // Compute a node_id for each flat zone
    template <class I, class J>
    [[gnu::noinline]] std::size_t alphatree_create_nodemap(I node_map, J zpar)
    {
      std::size_t node_count = 0;
      mln_foreach (auto px, node_map.pixels())
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
    void alphatree_compute_flatzones(E& edges, J zpar)
    {
      canvas::impl::union_find_init_par(zpar);

      while (!edges.empty() && edges.top() == 0)
      {
        const auto [p, q, w] = edges.pop();
        auto rp              = canvas::impl::zfindroot(zpar, p);
        auto rq              = canvas::impl::zfindroot(zpar, q);
        if (rp != rq) // Merge set
          zpar(rq) = rp;
      }
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


    template <class I, class N, class F>
    auto alphatree_compute_edges(I input, N nbh, F distance)
    {
      static_assert(is_a_v<I, mln::details::Image>);
      using V    = image_value_t<I>;
      auto edges = alphatree_edges<I, N, std::invoke_result_t<F, V, V>>();
      auto dom   = input.domain();
      mln_foreach (auto p, dom)
      {
        std::size_t i = 0;
        for (auto n : nbh.after(p))
        {
          if (dom.has(n))
            edges.push(i, distance(input(p), input(n)), p);
          ++i;
        }
      }
      edges.on_finish_insert();
      return edges;
    }

    //
    //
    template <class E, class I, class W>
    std::size_t alphatree_compute_hierarchy(E& edges, I node_map,         //
                                            std::size_t       node_count, //
                                            std::vector<int>& par,        //
                                            std::vector<W>&   levels)
    {
      static_assert(mln::is_a<I, mln::details::Image>());


      std::vector<int> zpar(node_count);  // Parent of ufind structure
      std::vector<int> links(node_count); // Node index in the alpha tree

      std::iota(std::begin(zpar), std::end(zpar), 0);
      std::iota(std::begin(links), std::end(links), 0);

      while (!edges.empty())
      {
        auto [p, q, w] = edges.pop();
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

    // 1. Get the list of edges
    auto edges = internal::alphatree_compute_edges(std::move(input), std::move(nbh), std::move(distance));

    std::size_t              flatzones_count;
    image_ch_value_t<I, int> node_map = imchvalue<int>(input).set_init_value(-1);
    {
      image_ch_value_t<I, P> zpar = imchvalue<P>(input);
      // 2. Compute flat zone of the image
      internal::alphatree_compute_flatzones(edges, zpar);

      // 3. Compute a node_id for each flat zone
      flatzones_count = internal::alphatree_create_nodemap(node_map, zpar);
    }

    std::size_t node_count = flatzones_count;

    std::vector<int> par(node_count);
    std::vector<W>   levels(node_count, 0);
    // 4. Compute the hierarchy
    {
      std::iota(std::begin(par), std::end(par), 0);
      node_count = internal::alphatree_compute_hierarchy(edges, node_map, node_count, par, levels);
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

} // namespace mln::morpho
