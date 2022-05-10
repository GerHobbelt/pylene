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
    struct waterfall_visitor
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
    };
  } // namespace details

  template <typename I, typename N>
  auto waterfall(I ima, N nbh)
  {
    static_assert(mln::is_a_v<I, mln::details::Image>);
    static_assert(mln::is_a_v<N, mln::details::Neighborhood>);

    mln_entering("mln::morpho::waterfall");
    image_build_error_code     err = IMAGE_BUILD_OK;
    details::waterfall_visitor viz;
    auto                       output = imchvalue<int>(ima) //
                      .adjust(nbh)
                      .set_init_value(-1)
                      .get_status(&err)
                      .build();

    if (err == IMAGE_BUILD_OK)
    {
      impl::watershed_partition(ima, nbh, output, viz);
    }
    else
    {
      mln::trace::warn("[Performance] The extension is not wide enough");
      auto out = view::value_extended(output, -1);
      impl::watershed_partition(ima, nbh, out, viz);
    }

    component_tree<int> t;
    internal::alphatree_reorder_nodes(viz.parent.data(), viz.values.data(), viz.parent.size());
    std::tie(t.parent, t.values) = internal::canonize_component_tree(viz.parent, viz.values, output);

    return std::make_pair(std::move(t), std::move(output));
  }
} // namespace mln::morpho