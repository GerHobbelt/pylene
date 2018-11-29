#ifndef MLN_MORPHO_RANK_FILTER_HPP
#define MLN_MORPHO_RANK_FILTER_HPP

#include <mln/accu/accumulators/h_rank.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/image/image.hpp>
#include <mln/kernelv2/kernel.hpp>
#include <mln/morpho/se/se.hpp>

/// \file

namespace mln
{

  namespace morpho
  {

    /// \ingroup morpho
    /// \brief Rank filter
    ///
    /// A rank filter is non-linerar filter using the value ordering to
    /// compute the filtered value. It computes:
    /// \f[
    /// g(x) = r(\{ f(y) \in \mathcal{B}_x) \})
    /// \f]
    /// where \p r returns the 𝑟-th value of the set of pixels of the
    /// structuring element 𝑩 centered in 𝑥.
    ///
    ///
    /// \param ima Input image 𝑓
    /// \param se  Structuring element
    /// \param out (optional) Output image 𝑔 so store the result.
    /// \tparam Ratio A std::ratio type that defines the rank of the filtered value.
    template <class Ratio, class I, class SE, class OutputImage>
    OutputImage& rank_filter(const Image<I>& ima, const StructuringElement<SE>& se, Image<OutputImage>& out);

    /// \ingroup morpho
    /// \overload  OutputImage& rank_filter(const Image<I>&, const StructuringElement<SE>&, Image<OutputImage>&);
    template <class Ratio, class I, class SE>
    mln_concrete(I) rank_filter(const Image<I>& ima, const StructuringElement<SE>& se);

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
      template <class Ratio, class I, class SE, class J>
      void rank_filter(const I& ima, const SE& nbh, J& out, std::true_type __is_incremental__)
      {
        namespace ker = mln::kernel;
        (void)__is_incremental__;

        typedef accu::accumulators::h_rank<mln_value(I), Ratio> ACCU;
        ker::Aggregate<ACCU>                                    A;
        ker::Point                                              p;
        ker::Neighbor                                           n;
        auto                                                    f    = ker::make_image_expr<0>(ima);
        auto                                                    g    = ker::make_image_expr<1>(out);
        auto                                                    expr = kernel::declare(g(p) = A(f(n)));
        ker::execute_incremental(expr, nbh);
      }

      /// \brief Basic implementation
      /// This specialization is used when:
      /// * Either the SE is not incremental or nor the feature has the `untake` method.
      template <class Ratio, class I, class SE, class J>
      void rank_filter(const I& ima, const SE& nbh, J& out, std::false_type __is_incremental__)
      {
        namespace ker = mln::kernel;
        (void)__is_incremental__;

        typedef accu::accumulators::h_rank<mln_value(I), Ratio> ACCU;
        ker::Aggregate<ACCU>                                    A;
        ker::Point                                              p;
        ker::Neighbor                                           n;
        auto                                                    f = ker::make_image_expr<0>(ima);
        auto                                                    g = ker::make_image_expr<1>(out);

        auto expr = kernel::declare(g(p) = A(f(n)));
        ker::execute(expr, nbh);
      }

    } // end of namespace mln::morpho::impl

    template <class Ratio, class I, class SE, class OutputImage>
    OutputImage& rank_filter(const Image<I>& ima_, const StructuringElement<SE>& se_, Image<OutputImage>& output_)
    {
      mln_entering("mln::morpho::rank_filter");

      using has_extension = typename image_has_extension<I>::type;
      const I&     ima    = exact(ima_);
      const SE&    se     = exact(se_);
      OutputImage& out    = exact(output_);

      static_assert(has_extension::value, "You must ensure that the image has an extension wide enough to hold the SE");

      using is_se_incremental    = typename neighborhood_traits<SE>::is_incremental;
      using is_image_incremental = typename iterator_traits<mln_pxter(I)>::has_NL;
      using is_incremental = std::integral_constant<bool, is_se_incremental::value and is_image_incremental::value>;

      if (extension::need_adjust(ima, se))
      {
        mln::trace::warn("You must ensure that the image has an extension wide enough to hold the SE");
        std::abort();
      }
      else
      {
        impl::rank_filter<Ratio>(ima, se, out, is_incremental());
      }

      mln_exiting();
      return out;
    }

    template <class Ratio, class I, class SE>
    mln_concrete(I) rank_filter(const Image<I>& ima, const StructuringElement<SE>& se)
    {
      mln_concrete(I) out = imconcretize(exact(ima));
      rank_filter<Ratio>(ima, se, out);
      return out;
    }

  } // end of namespace mln::morpho

} // end of namespace mln

#endif // ! MLN_MORPHO_RANK_FILTER_HPP
