#ifndef MLN_MORPHO_STRUCTURAL_DILATE_HPP
#define MLN_MORPHO_STRUCTURAL_DILATE_HPP

#include <mln/accu/accumulators/h_infsup.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/morpho/canvas/dilation_like.hpp>

/// \file

namespace mln
{
  namespace morpho
  {
    namespace structural
    {

      /// \ingroup morpho
      /// \brief Dilation by a structuring element.
      ///
      /// Given a structuring element 𝐵, the dilation \f$\varepsilon(f)\f$ of the input
      /// image 𝑓 is defined as:
      /// \f[
      /// \varepsilon(f)(x) = \bigvee \{ \, f(y), y \in B_x \, \}
      /// \f]
      ///
      /// + If the optional \p out image is provided, it must be wide enough to store
      ///   the results (the function does not perform any resizing).
      ///
      /// + If a optional \p cmp function is provided, the algorithm
      ///   will internally do an unqualified call to `sup(x, y,
      ///   cmp)`.  The default is the product-order so that it works
      ///   for vectorial type as well.
      ///
      /// \param[in] ima Input image \p f
      /// \param[in] se Structuring element/Neighborhood/Window \p B to look around.
      /// \param[out] out (optional) Output image \p g to write in.
      /// \param[in] cmp (optional) Comparaison function.
      ///
      template <class I, class SE, class OutputImage, class Compare = productorder_less<mln_value(I)>>
      OutputImage& dilate(const Image<I>& ima, const StructuringElement<SE>& se, Image<OutputImage>& out,
                          Compare cmp = Compare());

      /// \ingroup morpho
      /// \overload OutputImage& dilate(const Image<I>&, const StructuringElement<SE>&, Image<OutputImage>&, Compare
      /// cmp)
      template <class I, class SE, class Compare = productorder_less<mln_value(I)>>
      mln_concrete(I) dilate(const Image<I>& ima, const StructuringElement<SE>& se, Compare cmp = Compare());

      /******************************************/
      /****          Implementation          ****/
      /******************************************/

      namespace internal
      {

        template <class T, class Compare>
        struct dilate_traits_base
        {
          dilate_traits_base(Compare cmp)
            : accu_sup(cmp)
            , m_cmp(std::move(cmp))
          {
          }

          const mln::accu::accumulators::sup<T, Compare> accu_sup;
          auto                                           sup(T x, T y) const { return mln::sup(x, y, m_cmp); }
          static constexpr T                             zero() { return value_traits<T, Compare>::inf(); }

        private:
          Compare m_cmp;
        };

        template <class T, class Compare, class Enable = void>
        struct dilate_traits : dilate_traits_base<T, Compare>
        {
          using dilate_traits_base<T, Compare>::dilate_traits_base;
          using support_incremental = std::false_type;
        };

        template <class T>
        struct dilate_traits<T, productorder_less<T>, std::enable_if_t<std::is_integral<T>::value>>
          : dilate_traits_base<T, productorder_less<T>>
        {
          using support_incremental = std::true_type;

          dilate_traits(productorder_less<T> cmp)
            : dilate_traits_base<T, productorder_less<T>>(cmp)
          {
          }

          const mln::accu::accumulators::h_sup<T> accu_hsup;
        };
      }

      template <class I, class SE, class OutputImage, class Compare>
      OutputImage& dilate(const Image<I>& ima, const StructuringElement<SE>& nbh, Image<OutputImage>& output,
                          Compare cmp)
      {
        mln_entering("mln::morpho::structural::dilate");

        internal::dilate_traits<mln_value(I), Compare> __op__(cmp);
        morpho::canvas::dilation_like(ima, nbh, output, __op__);

        mln_exiting();
        return exact(output);
      }

      template <class I, class SE, class Compare>
      mln_concrete(I) dilate(const Image<I>& ima, const StructuringElement<SE>& nbh, Compare cmp)
      {
        mln_concrete(I) out = imconcretize(exact(ima));
        dilate(ima, nbh, out, cmp);
        return out;
      }

    } // end of namespace mln::morpho::structural
  }   // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_STRUCTURAL_DILATE_HPP
