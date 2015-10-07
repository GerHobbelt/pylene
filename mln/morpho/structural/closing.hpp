#ifndef MLN_MORPHO_STRUCTURAL_CLOSING_HPP
# define MLN_MORPHO_STRUCTURAL_CLOSING_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/algorithm/transform.hpp>
# include <mln/core/trace.hpp>
# include <mln/morpho/se/se.hpp>
# include <mln/morpho/structural/erode.hpp>
# include <mln/morpho/structural/dilate.hpp>

/// \file

namespace mln
{

  namespace morpho
  {

    namespace structural
    {
      /// \ingroup morpho
      /// \brief Compute the structural morphological closing.
      ///
      /// The structurual morphological closing γ of \p f by a structuring
      /// element 𝓑
      /// \f[
      /// \gamma(f) = \varepsilon_\mathcal{B}(\delta_\mathcal{B}(f))
      /// \f]
      /// where ε denotes the erosion and δ the dilation.
      ///
      ///
      /// + If the optional \p out image is provided, it must be wide enough to store
      ///   the results (the function does not perform any resizing).
      ///
      /// + If a optional \p cmp function is provided, the algorithm
      ///   will internally do an unqualified call to `sup(x, y, cmp)`
      ///   and `inf(x, y,cmp)`.  The default is the product-order so
      ///   that it works for vectorial type as well.
      ///
      /// \param[in] input Input image \p f
      /// \param[in] se Structuring element/Neighborhood/Window \p B to look around.
      /// \param[out] out (optional) Output image \p g to write in.
      /// \param[in] cmp (optional) Comparaison function.
      ///
      template <class I, class SE, class Compare, class O>
      O&
      closing(const Image<I>& input,
              const StructuringElement<SE>& se,
              Compare cmp,
              Image<O>& out);

      /// \ingroup morpho
      /// \overload OutputImage& closing(const Image<I>&, const StructuringElement<SE>&, Compare cmp, Image<OutputImage>&)
      template <class I, class SE,
                class Compare = productorder_less<mln_value(I)> >
      mln_concrete(I)
      closing(const Image<I>& input,
              const StructuringElement<SE>& se,
              Compare cmp = Compare ());


      /*************************/
      /***  Implementation   ***/
      /*************************/

      namespace impl
      {

        // Version non-fast dilate - erode
        template <class I, class SE, class Compare, class J>
        void
        closing(const I& ima, const SE& nbh, Compare cmp, J& out)
        {
          auto d = morpho::structural::dilate(ima, nbh, cmp);
          morpho::structural::erode(d, nbh, out, cmp);
        }

      }

      template <class I, class SE, class Compare, class O>
      O&
      closing(const Image<I>& ima_,
              const StructuringElement<SE>& se_,
              Compare cmp,
              Image<O>& output)
      {
        mln::trace::entering("mln::morpho::closing");

        const I& ima = exact(ima_);
        const SE& se = exact(se_);
        O& out = exact(output);
        mln::morpho::structural::impl::closing(ima, se, cmp, out);

        mln::trace::exiting();
        return out;
      }


      template <class I, class SE, class Compare>
      mln_concrete(I)
      closing(const Image<I>& ima_,
              const StructuringElement<SE>& se,
              Compare cmp)
      {
        const I& ima = exact(ima_);

        mln_concrete(I) out = imconcretize(ima);
        mln::morpho::structural::closing(ima, se, cmp, out);

        return out;
      }

    }

  }

}

#endif // !MLN_MORPHO_STRUCTURAL_CLOSING_HPP
