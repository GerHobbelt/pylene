#pragma once

#include <mln/core/canvas/local_accumulation.hpp>
#include <mln/morpho/experimental/private/dilation_by_periodic_line.hpp>

namespace mln::morpho::experimental::details
{

  namespace impl
  {
    // In this step border managmenet has been handled
    // Dispatching to the best impl
    // They are order from more specialiazed to the more generalized

    // Specialized - Periodic line with box2d domain
    template <class I, class J, class ValueSet>
    void localmax(I in, J& out, ValueSet& vs, const mln::se::periodic_line2d& se, const mln::box2d& roi)
    {
      mln::trace::warn("[Performance] Running the specialization with perodic lines.");
      mln::morpho::details::dilation_by_periodic_line(in, out, se, vs.sup, roi);
    }

    // Specialized - Periodic line with box2d domain
    template <class I, class ValueSet>
    void localmax_inplace(I& inout, ValueSet& vs, const mln::se::periodic_line2d& se, const mln::box2d& roi)
    {
      mln::trace::warn("[Performance] Running the specialization with perodic lines.");
      mln::morpho::details::dilation_by_periodic_line(inout, inout, se, vs.sup, roi);
    }

    // Generic - Regular SE over a domain
    template <class I, class J, class ValueSet, class SE, class D>
    void localmax(I in, J& out, ValueSet& vs, const mln::details::StructuringElement<SE>& se, const D&)
    {
      if constexpr (SE::incremental::value && ValueSet::has_incremental_sup::value)
      {
        mln::canvas::LocalAccumulation alg = {vs.accu_incremental_sup, static_cast<const SE&>(se), in, out};
        alg.Execute();
      }
      else
      {
        mln::canvas::LocalAccumulation alg = {vs.accu_sup, static_cast<const SE&>(se), in, out};
        alg.Execute();
      }
    }
  } // namespace impl

  namespace details
  {
    template <class SE, class Domain, class = void>
    struct is_se_compatible_with : std::false_type
    {
    };

    template <class SE, class D>
    struct is_se_compatible_with<SE, D,
                                 std::void_t<decltype(std::declval<SE>().compute_input_region(std::declval<D>()),
                                                      std::declval<SE>().compute_output_region(std::declval<D>()))>>
      : std::true_type
    {
    };

    template <class SE, class Domain>
    static constexpr bool is_se_compatible_with_v = is_se_compatible_with<SE, Domain>::value;
  } // namespace details


  template <class I, class J, class SE, class BorderManager, class ValueSet>
  void localmax(I input, J& output, const SE& se, const BorderManager& bm, const ValueSet& vs)
  {
    using D = image_domain_t<I>;

    // To enable when we can concept check that domain are comparable
    // assert(input.domain() == output.domain());
    auto roi = input.domain();

    if constexpr (SE::decomposable::value && details::is_se_compatible_with_v<SE, D>)
    {
      if (se.is_decomposable())
      {
        mln::trace::warn("[Performance] Running the specialization with SE decomposition");
        auto ses       = se.decompose();
        auto tmp       = bm.create_temporary_image(input, se, roi);
        auto input_roi = se.compute_input_region(roi);
        for (auto&& se : ses)
        {
          auto wroi = se.compute_output_region(input_roi);
          impl::localmax_inplace(tmp, vs, se, wroi);
        }
        mln::paste(tmp, roi, output);
        return;
      }
    }

    auto [imas, ses] = bm.manage(input, se);
    std::visit([&](auto&& ima, auto&& se) { impl::localmax(ima, output, vs, se, roi); }, imas, ses);
  }

} // namespace mln::morpho::experimental::details
