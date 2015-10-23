#ifndef MLN_MORPHO_HIT_OR_MISS_HPP
# define MLN_MORPHO_HIT_OR_MISS_HPP

# include <mln/core/image/image.hpp>
# include <mln/morpho/structural/dilate.hpp>
# include <mln/morpho/structural/erode.hpp>

/// \file
/// \ingroup morpho


namespace mln
{

  namespace morpho
  {

    /// \ingroup morpho
    /// \brief (Unconstrained) Hit-or-miss transform
    ///
    /// The hit-or-miss transform is non-linerar filter used to detect
    /// pattern in images. It is defined as:
    ///
    /// \f[
    /// UHTM_\mathcal{B}(f)(x) =
    /// \begin{cases}
    /// \varepsilon_{B_{fg}}(x) - \delta_{B_{bg}}(x) & \mathrm{if} \varepsilon_{B_{fg}}(x) > \delta_{B_{bg}}(x)\newline
    /// 0 & otherwise
    /// \end{cases}
    /// \f]
    /// where \f$ \mathcal{B} = (B_{fg}, B_{bg}) \f$ is the composite structuring element
    /// composed by the two disjoint foreground and background SEs.
    ///
    /// \param ima Input image 𝑓
    /// \param se_hit  Structuring element for foreground
    /// \param se_miss  Structuring element for background
    /// \param out (optional) Output image 𝑔 so store the result.
    template <class I, class SEh, class SEm, class OutputImage>
    OutputImage&
    hit_or_miss(const Image<I>& input,
                const StructuringElement<SEh>& se_hit,
                const StructuringElement<SEm>& se_miss,
                Image<OutputImage>& out);

    /// \ingroup morpho
    /// \overload  OutputImage& hit_or_miss(const Image<I>&, const StructuringElement<SEh>&, const StructuringElement<SEm>&, Image<OutputImage>&);
    template <class I, class SEh, class SEm>
    mln_concrete(I)
    hit_or_miss(const Image<I>& input,
                const StructuringElement<SEh>& se_hit,
                const StructuringElement<SEm>& se_miss);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/


    namespace impl
    {

      // Gray level impl
      template <class I, class SEh, class SEm, class OutputImage, typename V>
      OutputImage&
      unconstrained_hit_or_miss(const I& f,
                                const SEh& seh,
                                const SEm& sem,
                                OutputImage& out,
                                V __v__)
      {
        (void) __v__;
        mln_entering("mln::morpho::hit_or_miss(generic)");

        auto ero = morpho::structural::erode(f, seh);
        auto dil = morpho::structural::dilate(f, sem);

        mln_value(I) z = literal::zero;
        auto res = where(dil < ero, ero - dil, z);
        copy(res, out);

        mln_exiting();
        return out;
      }

      // Boolean case
      template <class I, class SEh, class SEm, class OutputImage>
      OutputImage&
      unconstrained_hit_or_miss(const I& f,
                                const SEh& seh,
                                const SEm& sem,
                                OutputImage& out,
                                bool __v__)
      {
        (void) __v__;
        mln_entering("mln::morpho::hit_or_miss(boolean)");

        auto ero = morpho::structural::erode(f, seh);
        auto dil = morpho::structural::dilate(f, sem);

        auto res = land(ero, lnot(dil));
        copy(res, out);

        mln_exiting();
        return out;
      }



    }

    template <class I, class SEh, class SEm, class OutputImage>
    OutputImage&
    hit_or_miss(const Image<I>& input,
                const StructuringElement<SEh>& se_hit,
                const StructuringElement<SEm>& se_miss,
                Image<OutputImage>& out)
    {
      impl::unconstrained_hit_or_miss(exact(input), exact(se_hit), exact(se_miss), exact(out), mln_value(I) ());
      return exact(out);
    }

    template <class I, class SEh, class SEm>
    mln_concrete(I)
    hit_or_miss(const Image<I>& input,
                const StructuringElement<SEh>& se_hit,
                const StructuringElement<SEm>& se_miss)
    {
      const I& f = exact(input);
      mln_concrete(I) out = imconcretize(f);
      impl::unconstrained_hit_or_miss(f, exact(se_hit), exact(se_miss), out, mln_value(I) ());
      return out;
    }



  } // end of namespace mln::morpho

} // end of namespace mln

#endif // ! MLN_MORPHO_HIT_OR_MISS_HPP
