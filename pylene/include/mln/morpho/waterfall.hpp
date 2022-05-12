#pragma once

#include <mln/core/neighborhood/c4c8.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/watershed.hpp>

#include <map>
#include <tuple>
#include <vector>

namespace mln::morpho
{
  namespace details
  {
    /*struct waterfall_visitor
    {
      void on_init(int nlbl) noexcept
      {
        // Result initialization
        m_nlbl = nlbl + 1;
        values.resize(m_nlbl, 0);
        std::iota(parent.begin(), parent.end(), 0);
        parent.resize(m_nlbl);

        // Helpers
        m_save = m_nlbl;
        m_zpar.resize(m_nlbl);
        std::iota(m_zpar.begin(), m_zpar.end(), 0);
        m_diameter.resize(m_nlbl, 0);
        m_roots.resize(m_nlbl);
      }

      bool on_visit(int p, int q, mln::dontcare_t, mln::dontcare_t) noexcept
      {
        using mln::morpho::canvas::impl::zfindroot;

        int rp = zfindroot(m_zpar.data(), p);
        int rq = zfindroot(m_zpar.data(), q);
        if (rp != rq)
        {
          m_zpar[rp] = rq;
          int min, max; // Using tie instead of SB due to lifetime error
          std::tie(min, max) = std::minmax(m_diameter[m_roots[rp]], m_diameter[m_roots[rq]]);
          m_diameter.push_back(std::max(min + 1, max));
          m_mst.emplace_back(p, q, min + 1);
          m_roots[rp] = m_nlbl;
          m_roots[rq] = m_nlbl;
          m_nlbl++;
        }
        return false;
      }

      void on_finalize() noexcept
      {
        using mln::morpho::canvas::impl::zfindroot;

        std::ranges::sort(m_mst, [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });

        m_nlbl = m_save;
        std::iota(m_zpar.begin(), m_zpar.end(), 0);
        std::iota(m_roots.begin(), m_roots.end(), 0);
        for (auto [p, q, w] : m_mst)
        {
          // No test : we are traversing the MST
          int rp     = zfindroot(m_zpar.data(), p);
          int rq     = zfindroot(m_zpar.data(), q);
          m_zpar[rp] = rq;

          // BPTAO of the reweighted MST
          parent.push_back(m_nlbl);
          parent[m_roots[rp]] = m_nlbl;
          parent[m_roots[rq]] = m_nlbl;

          // Update roots in the parenthood
          m_roots[rp] = m_nlbl;
          m_roots[rq] = m_nlbl;

          values.push_back(w);
          m_nlbl++;
        }
      }

    public:
      std::vector<int> values;
      std::vector<int> parent;

    private:
      std::vector<int>                       m_zpar;
      std::vector<int>                       m_diameter;
      std::vector<std::tuple<int, int, int>> m_mst;
      int                                    m_nlbl;
      int                                    m_save;
      std::vector<int>                       m_roots;
    };*/

    template <typename I, typename O, typename N>
    std::vector<std::vector<std::pair<int, image_value_t<I>>>> watershed_rag(I input, O output, N nbh, int& nlabel)
    {
      using V = image_value_t<I>;
      using P = image_point_t<I>;

      nlabel = mln::labeling::impl::local_minima(input, nbh, output, std::less<V>()) + 1;

      std::vector<std::vector<std::pair<int, image_value_t<I>>>> rag(nlabel);

      constexpr auto impl_type = mln::morpho::details::pqueue_impl::linked_list;
      mln::morpho::details::pqueue_fifo<I, impl_type, /* reversed = */ true> pqueue(input);
      {
        output.extension().fill(-1);
        mln_foreach (auto pix, output.pixels())
        {
          if (pix.val() <= 0)
            continue;
          for (auto p : nbh(pix))
          {
            if (p.val() == 0)
            {
              pqueue.push(input(pix.point()), pix.point());
              break;
            }
          }
        }

        const auto add_edge = [&rag, &input, &output](const P& p, const P& q) {
          const auto [lbl_min, lbl_max] = std::minmax(output(p), output(q));
          V   max_w                     = std::max(input(p), input(q));
          int i                         = 0;
          while (i < static_cast<int>(rag[lbl_min].size()))
          {
            auto [out_v, e] = rag[lbl_min][i];
            if (out_v == lbl_max)
            {
              rag[lbl_min][i].second = std::min(e, max_w);
              break;
            }
            i++;
          }
          if (i == static_cast<int>(rag[lbl_min].size()))
            rag[lbl_min].emplace_back(lbl_max, max_w);
        };

        while (!pqueue.empty())
        {
          auto [lvl, p] = pqueue.top();
          pqueue.pop();
          for (auto q : nbh(p))
          {
            if (output.at(q) == 0) // If extension then -1
            {
              output(q) = output(p);
              pqueue.push(input(q), q);
            }
            else if (output.at(q) > 0 && output.at(q) != output.at(p))
              add_edge(p, q);
          }
        }
      }

      return rag;
    }

    template <typename V>
    std::vector<std::tuple<int, int, int>> mst_waterfall(const std::vector<std::vector<std::pair<int, V>>>& rag)
    {
      std::vector<std::tuple<int, int, V>> flatten_graph;
      for (int in = 0; in < static_cast<int>(rag.size()); in++)
      {
        for (auto [out, w] : rag[in])
          flatten_graph.emplace_back(in, out, w);
      }
      std::ranges::sort(flatten_graph, [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });

      // Kruskal
      int                                    nlbl = rag.size();
      std::vector<int>                       zpar(nlbl);
      std::vector<int>                       diameter(nlbl);
      std::vector<std::tuple<int, int, int>> mst;
      std::vector<int>                       roots(nlbl);
      std::iota(zpar.begin(), zpar.end(), 0);
      std::iota(roots.begin(), roots.end(), 0);
      using mln::morpho::canvas::impl::zfindroot;
      for (auto [p, q, _] : flatten_graph)
      {
        int rp = zfindroot(zpar.data(), p);
        int rq = zfindroot(zpar.data(), q);
        if (rp != rq)
        {
          zpar[rp] = rq;
          int min, max; // Using tie instead of SB due to lifetime error
          std::tie(min, max) = std::minmax(diameter[roots[rp]], diameter[roots[rq]]);
          diameter.push_back(std::max(min + 1, max));
          mst.emplace_back(p, q, min + 1);
          roots[rp] = nlbl;
          roots[rq] = nlbl;
          nlbl++;
        }
      }
      return mst;
    }

    template <typename N>
    component_tree<int> waterfall_from_mst(std::vector<std::tuple<int, int, int>>& mst, int n_vertices, N& nm)
    {
      using mln::morpho::canvas::impl::zfindroot;

      std::vector<int> value(n_vertices, 0);
      std::vector<int> parent(n_vertices);
      std::vector<int> roots(n_vertices);
      std::vector<int> zpar(n_vertices);

      std::iota(parent.begin(), parent.end(), 0);
      std::iota(zpar.begin(), zpar.end(), 0);
      std::iota(roots.begin(), roots.end(), 0);

      std::ranges::sort(mst, [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });

      int nlbl = n_vertices;
      for (auto [p, q, w] : mst)
      {
        // No test : we are traversing the MST
        int rp   = zfindroot(zpar.data(), p);
        int rq   = zfindroot(zpar.data(), q);
        zpar[rp] = rq;

        // BPTAO of the reweighted MST
        parent.push_back(nlbl);
        parent[roots[rp]] = nlbl;
        parent[roots[rq]] = nlbl;

        // Update roots in the parenthood
        roots[rp] = nlbl;
        roots[rq] = nlbl;

        value.push_back(w);
        nlbl++;
      }


      internal::alphatree_reorder_nodes(parent.data(), value.data(), parent.size());
      component_tree<int> res;
      std::tie(res.parent, res.values) = internal::canonize_component_tree(parent, value, nm);

      return res;
    }
  } // namespace details

  namespace impl
  {
    template <typename I, typename N>
    std::pair<component_tree<int>, image_ch_value_t<I, int>> waterfall(I input, N nbh)
    {
      const auto process = [&input, &nbh]<typename O>(O output) {
        int        nlbl = 0;
        const auto rag  = details::watershed_rag(input, output, nbh, nlbl);
        auto       mst  = details::mst_waterfall(rag);
        return details::waterfall_from_mst(mst, nlbl, output);
      };

      image_build_error_code err    = IMAGE_BUILD_OK;
      auto                   output = imchvalue<int>(input) //
                        .adjust(nbh)
                        .set_init_value(-1)
                        .get_status(&err)
                        .build();

      component_tree<int> t;
      if (err == IMAGE_BUILD_OK)
      {
        t = process(output);
      }
      else
      {
        mln::trace::warn("[Performance] The extension is not wide enough");
        auto out = view::value_extended(output, -1);
        t        = process(out);
      }
      return {std::move(t), output};
    }
  } // namespace impl

  template <typename I, typename N>
  auto waterfall(I ima, N nbh)
  {
    static_assert(mln::is_a_v<I, mln::details::Image>);
    static_assert(mln::is_a_v<N, mln::details::Neighborhood>);

    mln_entering("mln::morpho::waterfall");

    return impl::waterfall(ima, nbh);
  }
} // namespace mln::morpho