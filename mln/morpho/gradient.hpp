#ifndef MLN_MORPHO_GRADIENT_HPP
# define MLN_MORPHO_GRADIENT_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/algorithm/transform.hpp>
# include <mln/core/extension/extension.hpp>
# include <mln/core/extension/mirror.hpp>
# include <mln/core/math_ops.hpp>
# include <mln/core/trace.hpp>
# include <mln/morpho/se/se.hpp>
# include <mln/morpho/erode.hpp>
# include <mln/morpho/dilate.hpp>
# include <mln/kernel/kernel.hpp>
# include <mln/kernel/aggregate/sup.hpp>
# include <mln/kernel/aggregate/inf.hpp>
# include <mln/kernel/function.hpp>


namespace mln
{

  namespace morpho
  {

    /// \brief Compute the Beucher gradient.
    ///
    /// The beucher gradient is defined as:
    /// \f[
    ///  |\Nabla u(p)| = \left| \bigvee_{q \in \mathcal{N}(p)} f(q) -
    ///    |\bigwedge_{q \in \mathcal{N}(p)} f(q) \right|
    /// \f]
    ///
    /// \param[in] input        Input image.
    /// \param[in] se   Neighborhood/SE/Window to look around.
    /// \param[in] cmp  (optional) Comparaison function. The method internally does an
    ///                 unqualified call to `inf(x, y, cmp)` and `sup(x, y, cmp)`. Default
    ///                 is the product-order.
    /// \param[in] norm (optional) Norm function used in \f$|x - y|\f$. The default is
    ///                  the ℓ₂-norm.
    /// \param[out] out (optional) Output image to write in.
    template <class I, class SE,
              class Compare = productorder_less<mln_value(I)>,
              class NormFunction = mln::functional::l2norm_t<mln_value(I)> >
    mln_ch_value(I, typename std::result_of<NormFunction(mln_value(I))>::type)
    gradient(const Image<I>& input,
	     const StructuringElement<SE>& se,
             Compare cmp = Compare (),
             NormFunction norm = NormFunction ());

    template <class I, class SE, class Compare, class NormFunction, class O>
    O&
    gradient(const Image<I>& input,
	     const StructuringElement<SE>& se,
             Compare cmp,
             NormFunction norm,
             Image<O>& out);

    /*************************/
    /***  Implementation   ***/
    /*************************/

    namespace impl
    {
      // Implementation notes:
      // * if the input image has a mirrorizable extension and the extension is wide enough
      //    => mirrorize and call the kernel
      // * otherwise
      //    => call dilate - erose

      // Version Fast that does one pass.
      template <class I, class SE, class Compare, class Norm, class J>
      void
      gradient_1(const I& ima, const SE& nbh, Compare cmp, Norm norm, J& out)
      {
        namespace ker = mln::kernel;
        using mln::kernel::placeholders::f;
        using mln::kernel::placeholders::g;
        using mln::kernel::placeholders::p;
        using mln::kernel::placeholders::n;


        ker::aggregate::Sup_t<Compare> Sup(cmp);
        ker::aggregate::Inf_t<Compare> Inf(cmp);
        ker::Function<Norm> D = {norm};

        auto expr = (g(p) = D(Sup(f(n)) - Inf(f(n))));
        ker::execute(expr, nbh, ima, out);
      }

      // Version non-fast dilate - erode
      template <class I, class SE, class Compare, class Norm, class J>
      void
      gradient_2(const I& ima, const SE& nbh, Compare cmp, Norm norm, J& out)
      {
        namespace ker = mln::kernel;
        using namespace mln::kernel::placeholders;

        auto d = morpho::dilate(ima, nbh, cmp);
        auto e = morpho::erode(ima, nbh, cmp);

        transform(d-e, norm, out);
      }

      template <class I, class SE, class Compare, class Norm, class J>
      void
      gradient_0(const I& ima, const SE& nbh, Compare cmp, Norm norm,
                 J& out, std::true_type has_mirrorizable_extension)
      {
        (void) has_mirrorizable_extension;

        if (extension::need_adjust(ima, nbh)) {
          mln::trace::warn("Slow version because input image extension is not wide enough.");
          gradient_2(ima, nbh, cmp, norm, out);
        } else {
          extension::mirror(ima);
          gradient_1(ima, nbh, cmp, norm, out);
        }
      }

      template <class I, class SE, class Compare, class Norm, class J>
      void
      gradient_0(const I& ima, const SE& nbh, Compare cmp, Norm norm,
                 J& out, std::false_type has_mirrorizable_extension)
      {
        (void) has_mirrorizable_extension;
        mln::trace::warn("Slow version because input image has no mirrorizable extension.");

        gradient_2(ima, nbh, cmp, norm, out);
      }

    }

    template <class I, class SE, class Compare, class Norm, class O>
    O&
    gradient(const Image<I>& ima_,
             const StructuringElement<SE>& se_,
             Compare cmp,
             Norm norm,
             Image<O>& output)
    {
      mln::trace::entering("mln::morpho::gradient");

      const I& ima = exact(ima_);
      const SE& se = exact(se_);
      O& out = exact(output);


      mln::morpho::impl::gradient_0(ima, se, cmp, norm, out,
                                    typename image_extension_traits<I>::support_mirror ());

      mln::trace::exiting();
      return out;
    }


    template <class I, class SE, class Compare, class Norm>
    mln_ch_value(I, typename std::result_of<Norm(mln_value(I))>::type)
    gradient(const Image<I>& ima_,
             const StructuringElement<SE>& se,
             Compare cmp, Norm norm)
    {
      const I& ima = exact(ima_);

      typedef mln_value(I) V;
      typedef typename std::result_of<Norm(V)>::type result_type;

      mln_ch_value(I, result_type) out = imchvalue<result_type> (ima);
      gradient(ima, se, cmp, norm, out);

      return out;
    }


    /*
    namespace impl
    {

      template <class I, class N, class O, class Compare>
      void
      gradient(const I& input,
	       const N& nbh,
	       O& out,
	       const Compare& cmp)
      {
	typedef accu::accumulator::minmax<mln_value(I), Compare> A;
	A acc(cmp);
	accfpfn(input, nbh, out, true, acc,
		[] (const A& acc) {
		  return extract(accu, mln::accu::features::max ()) -
		    extract(accu, mln::accu::features::min ());
		});
      }


    }


    template <class I, class N, class O, class Compare>
    O&
    gradient(const Image<I>& input,
	     const Neighborhood<N>& nbh,
	     Image<O>& out,
	     const Compare& cmp)
    {
      impl::gradient(exact(input), exact(nbh), exact(out), cmp);
      return exact(out);
    }

    template <class I, class N, class O, class Compare>
    mln_concrete(I)
    gradient(const Image<I>& input,
	     const Neighborhood<N>& nbh,
	     const Compare& cmp)
    {
      mln_concrete(I) out;
      resize(out, input);
      impl::gradient(exact(input), exact(nbh), out, cmp);
      return exact(out);
    }

    */
  }

}

#endif // !MLN_MORPHO_GRADIENT_HPP
