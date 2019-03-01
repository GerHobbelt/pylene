#pragma once

#include <mln/core/algorithm/copy.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>
#include <mln/kernelv2/kernel.hpp>
#include <mln/morpho/canvas/private/dilation_by_periodic_line.hpp>
#include <mln/morpho/se/se.hpp>


namespace mln
{

  namespace morpho
  {

    namespace canvas
    {

      //
      // struct dilation_like_operations_traits
      // {
      //   A0 incremental_aggregate;
      //   A1 aggregate;
      //   ... zero() constexpr;
      //   T sup(T x, T, y) const;
      // };

      template <class I, class SE, class J, class OpTraits>
      void dilation_like(const Image<I>& ima, const StructuringElement<SE>& nbh, Image<J>& output, OpTraits __op__);

      /******************************************/
      /****          Implementation          ****/
      /******************************************/

      namespace impl
      {

        /// \brief Incremental implementation that enables the dilation to be in
        /// a complexity that does not depend on the SE size.
        /// This specialization is used when:
        /// * The SE is incremental
        /// * The feature has the `untake` method
        template <class I, class SE, class J, class OpTraits>
        void dilate_like_1(const I& ima, const SE& nbh, J& out, OpTraits __op__, std::true_type __is_incremental__)
        {
          namespace ker = mln::kernel;
          (void)__is_incremental__;

          auto                           hsup = __op__.accu_hsup;
          ker::Aggregate<decltype(hsup)> A(hsup);
          ker::Point                     p;
          ker::Neighbor                  n;
          auto                           f    = ker::make_image_expr<0>(ima);
          auto                           g    = ker::make_image_expr<1>(out);
          auto                           expr = kernel::declare(g(p) = A(f(n)));
          ker::execute_incremental(expr, nbh);
        }

        /// \brief Basic implementation
        /// This specialization is used when:
        /// * Either the SE is not incremental or nor the feature has the `untake` method.
        template <class I, class SE, class J, class OpTraits>
        void dilate_like_1(const I& ima, const SE& nbh, J& out, OpTraits __op__, std::false_type __is_incremental__)
        {
          namespace ker = mln::kernel;
          (void)__is_incremental__;

          auto                          sup = __op__.accu_sup;
          ker::Aggregate<decltype(sup)> A(sup);
          ker::Point                    p;
          ker::Neighbor                 n;
          auto                          f = ker::make_image_expr<0>(ima);
          auto                          g = ker::make_image_expr<1>(out);

          auto expr = kernel::declare(g(p) = A(f(n)));
          ker::execute(expr, nbh);
        }

        template <class I, class SE, class J, class OpTraits>
        void dilate_like_0(const I& ima, const SE& nbh, J& out, OpTraits __op__, std::true_type __has_extension__)
        {
          (void)__has_extension__;

          mln_value(I) v = __op__.zero();

          using is_se_incremental    = typename neighborhood_traits<SE>::is_incremental;
          using is_accu_incremental  = typename OpTraits::support_incremental;
          using is_image_incremental = typename iterator_traits<mln_pxter(I)>::has_NL;
          using is_incremental =
              std::integral_constant<bool, is_se_incremental::value and is_accu_incremental::value and
                                               is_image_incremental::value>;

          if (not is_se_incremental::value)
            mln::trace::warn("Slow because SE is not incremental");
          else if (not is_accu_incremental::value)
            mln::trace::warn("Slow because the accu is not incremental with this image");
          else if (not is_image_incremental::value)
            mln::trace::warn("Slow because the image has no New Line Support");

          if (extension::need_adjust(ima, nbh))
          {
            mln::trace::warn("Slow version because input image extension is not wide enough.");
            dilate_like_1(extension::add_value_extension(ima, v), nbh, out, __op__, is_incremental());
          }
          else
          {
            extension::fill(ima, v);
            dilate_like_1(ima, nbh, out, __op__, is_incremental());
          }
        }

        template <class I, class SE, class J, class OpTraits>
        void dilate_like_0(const I& ima, const SE& nbh, J& out, OpTraits __op__, std::false_type __has_extension__)
        {
          (void)__has_extension__;
          mln::trace::warn("Slow version because input image has no extension.");

          using is_se_incremental    = typename neighborhood_traits<SE>::is_incremental;
          using is_accu_incremental  = typename OpTraits::support_incremental;
          using is_image_incremental = typename iterator_traits<mln_pxter(I)>::has_NL;
          using is_incremental =
              std::integral_constant<bool, is_se_incremental::value and is_accu_incremental::value and
                                               is_image_incremental::value>;

          if (not is_se_incremental::value)
            mln::trace::warn("Slow because SE is not incremental");
          else if (not is_accu_incremental::value)
            mln::trace::warn("Slow because the accu is not incremental with this image");
          else if (not is_image_incremental::value)
            mln::trace::warn("Slow because the image has no New Line Support");

          mln_value(I) v = __op__.zero();
          dilate_like_1(extension::add_value_extension(ima, v), nbh, out, __op__, is_incremental());
        }

      } // namespace impl

      namespace overload
      {
        // 2. Try a specific version of the algorithm.
        // This is the generic verion
        template <class I, class SE, class J, class OpTraits, class Domain>
        void dilation_dispatch_2(const I& input, const SE& se, J& output, OpTraits __op__, Domain __domain__)
        {
          (void)__domain__;
          impl::dilate_like_0(input, se, output, __op__, typename image_has_extension<I>::type());
        }

        // This is the version for regular 2D domain with periodic_line
        template <class I, class J, class OpTraits>
        void dilation_dispatch_2(const I& input, const se::periodic_line2d& line, J& output, OpTraits __op__,
                                 box2d __domain__)
        {
          (void)__domain__;
          (void)input;
          mln::copy(input, output);
          auto sup = [__op__](auto x, auto y) { return __op__.sup(x, y); };
          mln::morpho::internal::dilation_by_periodic_line(output, line, sup, __op__.zero());
        }

        // 2.bis Inplace version
        // Inplace generic version
        template <class I, class SE, class OpTraits, class Domain>
        void dilation_dispatch_2_inplace(I& f, const SE& se, OpTraits __op__, Domain __domain__)
        {
          (void)__domain__;
          // Fixme how to avoid copy and just swap images between iterations ?
          mln_concrete(I) temporary = imconcretize(f);
          impl::dilate_like_0(f, se, temporary, __op__, typename image_has_extension<I>::type());
          mln::copy(temporary, f);
        }

        // Inplace version for 2D regular domain with periodic line
        template <class I, class OpTraits>
        void dilation_dispatch_2_inplace(I& f, const se::periodic_line2d& line, OpTraits __op__, box2d __domain__)
        {
          (void)__domain__;
          auto sup = [__op__](auto x, auto y) { return __op__.sup(x, y); };
          mln::morpho::internal::dilation_by_periodic_line(f, line, sup, __op__.zero());
        }


        // 1. Try decomposing in smaller SE
        template <class I, class SE, class J, class OpTraits>
        void dilation_dispatch_1(const I& ima, const SE& nbh, J& output, OpTraits __op__,
                                 std::false_type __is_decomposable)
        {
          mln_entering("mln::morpho::canvas::dilation (not-decomposed)")(void) __is_decomposable;
          dilation_dispatch_2(ima, nbh, output, __op__, ima.domain());
        }

        template <class I, class SE, class J, class OpTraits>
        void dilation_dispatch_1(const I& input, const SE& nbh, J& output, OpTraits __op__,
                                 std::true_type __is_decomposable)
        {
          (void)__is_decomposable;
          if (nbh.decomposable())
          {
            mln_entering("mln::morpho::canvas::dilation (decomposed)") auto ses = nbh.decompose();
            mln::copy(input, output);
            for (auto&& se : ses)
              dilation_dispatch_2_inplace(output, se, __op__, output.domain());
          }
          else
          {
            mln_entering("mln::morpho::canvas::dilation (not-decomposed)")
                dilation_dispatch_2(input, nbh, output, __op__, output.domain());
          }
        }
      } // namespace overload

    } // namespace canvas

  } // namespace morpho

} // namespace mln

namespace mln
{

  namespace morpho
  {

    namespace canvas
    {

      template <class I, class SE, class J, class OpTraits>
      void dilation_like(const Image<I>& ima, const StructuringElement<SE>& nbh, Image<J>& output, OpTraits __op__)
      {
        overload::dilation_dispatch_1(exact(ima), exact(nbh), exact(output), __op__, typename SE::is_decomposable());
      }

    } // namespace canvas

  } // namespace morpho

} // namespace mln
