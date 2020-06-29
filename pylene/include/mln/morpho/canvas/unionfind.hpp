#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/algorithm/sort.hpp>

#include <mln/core/utils/dontcare.hpp> // detail::ignore_t
#include <mln/core/trace.hpp>

#include <range/v3/view/reverse.hpp>

namespace mln::morpho::canvas
{


  /// \brief Define a default union-find visitor.
  struct unionfind_visitor_base
  {
    using ignore_t = mln::dontcare_t;

    ///  \brief Called on every point \p during the make-set step.
    void on_make_set([[maybe_unused]] ignore_t  p) {}

    /// \brief Called during the merge-step.
    /// It returns the new root of the component (either p or q).
    ///
    /// Called when a component rooted in \p p merges with a component rooted in \p q through the point \p x.
    /// The component rooted in p is the last that has been updated (it is initially the current point being process).
    ///
    /// IF YOU WANT THE ROOT TO BE A POINT THAT HAS THE MINIMUM VALUE OF THE COMPONENT: RETURN p (it is usually what you
    /// want).  You want the root to be defined in another way (e.g. to a local maximum or using a rank criterion)
    ignore_t on_union([[maybe_unused]] ignore_t  p, [[maybe_unused]] ignore_t  rp,
                      [[maybe_unused]] ignore_t  q, [[maybe_unused]] ignore_t  rq) { return {}; }

    /// \brief Called at the end of the algorithm for each point
    ///
    /// \p p is any point in the domain, \p q is the root of the
    /// component \p p belongs to.
    void on_finish([[maybe_unused]] ignore_t  p, [[maybe_unused]] ignore_t  q) {}


    /// \brief Called after the merge or make_set step on the new root of the component
    /// Should return true if the component should not merge anymore
    bool test([[maybe_unused]] ignore_t  p) { return false; }
  };

  template <class I, class N, class UFViz, class Compare>
  void union_find(I input, N nbh, UFViz viz, Compare cmp);



  namespace impl
  {
    // Helper functions
    // Set par(p) = p for each p
    template <class I>
    [[gnu::noinline]] void union_find_init_par(I par) noexcept;


    // Get the canonical element of the set containing p0 and perform path compression
    template <class I>
    [[gnu::noinline]] //
    image_point_t<I>  //
    zfindroot(I& par, image_point_t<I> p0) noexcept;

    // Get the canonical element of the set containing p0 and perform path compression
    // This overload is for set reprensented by a buffer of indexes
    [[gnu::noinline]] int zfindroot(int* par, int x) noexcept;
  }


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace detail
  {
    enum class ufstatus : uint8_t
    {
      FAIL = 0,
      PASS = 1,
      NONE = 255,
    };

  }

  namespace impl
  {
    template <class I>
    [[gnu::noinline]] image_point_t<I> zfindroot(I& par, image_point_t<I> p0) noexcept
    {
      static_assert(mln::is_a<I, mln::details::Image>());

      image_point_t<I> r;
      // find root
      {
        auto p = p0, q = par(p);
        while (p != q)
        {
          p = q;
          q = par(p);
        }
        r = p;
      }

      // path compression
      for (auto p = p0; p != r;)
      {
        auto q = par(p);
        par(p) = r;
        p = q;
      }
      return r;
    }

    template <class P>
    struct uf_aux_data
    {
      P                                           par;
      mln::morpho::canvas::detail::ufstatus status;
    };


    template <class I, class J, class N, class UFViz>
    void union_find(I& par, J& status, N nbh, UFViz viz, const std::vector<image_point_t<I>>& S)
    {
      using mln::morpho::canvas::detail::ufstatus;

      // Forward pass
      {
        mln_entering("Union-find forward pass");
        for (auto p : ::ranges::views::reverse(S))
        {
          par(p) = p;
          viz.on_make_set(p);

          auto rp = p;
          ufstatus st = static_cast<ufstatus>(viz.test(p));
          for (auto n : nbh(p))
          {
            if (status.at(n) == ufstatus::NONE) // Out-of-range neighbor
              continue;

            auto rn = zfindroot(par, n);
            if (rn == rp) // Already the root
              continue;

            // Merging with a PASS component => so PASS (no need to merge)
            if (status(rn) == ufstatus::PASS)
            {
              st = ufstatus::PASS;
              continue;
            }

            auto new_root  = viz.on_union(p, rp, n, rn);
            par(rp)        = new_root;
            par(rn)        = new_root;
            rp             = new_root;
          }

          status(p) = static_cast<ufstatus>((bool)(st) || (bool)(viz.test(p)));
        }
      }

      // The root status always pass
      status(S.front()) = ufstatus::PASS;

      // Backward pass
      {
        mln_entering("Union-find Backward pass");
        for (auto p : S)
        {
          auto q = par(p);
          auto r = par(q);
          par(p) = r;
          viz.on_finish(p, r);
        }
      }
    }

    template <class I>
    [[gnu::noinline]] void union_find_init_par(I par) noexcept
    {
      mln_foreach(auto px, par.pixels())
        px.val() = px.point();
    }

  } // namespace impl



  template <class I, class N, class UFViz, class Compare>
  void union_find(I input, N nbh, UFViz viz, Compare cmp)
  {
    mln_entering("mln::morpho::canvas::union_find");

    using mln::morpho::canvas::detail::ufstatus;
    auto S = sort_points(input, cmp);

    using P  = image_point_t<I>;
    using St = image_ch_value_t<I, ufstatus>;

    // Status image
    auto bm     = mln::extension::bm::fill(ufstatus::NONE);
    auto status = bm.create_temporary_image<St>(nbh, input.domain());

    // Parent image
    image_ch_value_t<I, P> par = imchvalue<P>(input);

    impl::union_find(par, status, nbh, viz, S);
  }

} // namespace mln::morpho::canvas
