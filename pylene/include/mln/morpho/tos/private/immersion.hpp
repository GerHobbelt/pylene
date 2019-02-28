#ifndef MLN_MORPHO_TOS_PRIVATE_IMMERSION_HPP
#define MLN_MORPHO_TOS_PRIVATE_IMMERSION_HPP

#include <mln/core/image/image.hpp>
#include <mln/core/trace.hpp>
#include <mln/morpho/tos/private/irange.hpp>


namespace mln
{
  namespace morpho
  {
    namespace ToS
    {
      namespace impl
      {

        /// \brief Immerse a 2d or 3d image into twice-as-big 2d image
        /// where the in-between pixel are intervals.
        ///
        /// Given 4 2D-pixels:
        /// a   b
        /// c   d
        /// The resulting image is the intervaled valued image:
        ///
        ///      [a]       [  (a∧b)     (a∨b)]        [b]
        /// [(a∧c) (a∨c)]  [(a∧b∧c∧d) (a∨c∨d∨d)]  [(d∧b) (d∨b)]
        ///      [c]       [  (c∧d)     (c∨d)]        [d]
        ///
        /// \param[in] ima The input 2D image
        /// \param[in] cmp The comparaison function
        /// \return An intervaled valued 2D image
        template <class I>
        mln_ch_value(I, irange<mln_value(I)>) immersion(const Image<I>& ima);


        /******************************************/
        /****          Implementation          ****/
        /******************************************/


        template <int dim, class O>
        std::enable_if_t<(dim < (O::ndim - 1))> interpolation(O& output, mln_point(O) pmin, mln_point(O) pmax)
        {
          using P                                       = mln_point(O);
          mln::box<typename P::value_type, P::ndim> dom = {pmin, pmax};
          mln_foreach (auto p, dom)
          {
            auto prev = p;
            auto next = p;
            prev[dim]--;
            next[dim]++;
            output(p) = output(prev).extend(output(next));
          }
        }

        // 1d-Line case
        template <int dim, class I, class O>
        std::enable_if_t<dim == (O::ndim - 1)> immersion(const I& input, O& output, mln_point(I) pmin,
                                                         mln_point(I) pmax)
        {
          auto q = pmin / 2;
          auto p = pmin;
          auto a = input(q);

          output(p) = a;
          for (int i = pmin[dim] + 2; i < pmax[dim]; i += 2)
          {
            p[dim]++;
            q[dim]++;

            auto b    = input(q);
            auto mm   = std::minmax(a, b);
            output(p) = {mm.first, mm.second};
            p[dim]++;
            output(p) = b;
            a         = b;
          }
        }


        template <int dim, class I, class O>
            std::enable_if_t <
            dim<(O::ndim - 1)> immersion(const I& input, O& output, mln_point(I) pmin, mln_point(I) pmax)
        {
          immersion<dim + 1>(input, output, pmin, pmax);

          for (int i = pmin[dim] + 2, end = pmax[dim]; i < end; i += 2)
          {
            pmin[dim] = i;
            pmax[dim] = (i + 1);
            immersion<dim + 1>(input, output, pmin, pmax);

            pmin[dim] = (i - 1);
            pmax[dim] = i;
            interpolation<dim>(output, pmin, pmax);
          }
        }

        template <class I>
        mln_ch_value(I, irange<mln_value(I)>) immersion(const Image<I>& ima_)
        {
          static_assert(image_traits<I>::accessible::value, "Image must be accessible i.e. 'ima(p)'");

          mln_entering("mln::morpho::tos - immersion");

          using V = mln_value(I);
          using R = irange<V>;

          const I& ima = exact(ima_);


          auto output_domain = ima.domain();
          output_domain.pmin = output_domain.pmin * 2;
          output_domain.pmax = output_domain.pmax * 2 - 1;

          mln_ch_value(I, R) out(output_domain);

          immersion<0>(ima, out, output_domain.pmin, output_domain.pmax);
          return out;
        }

      } // namespace impl
    }   // namespace ToS
  }     // namespace morpho
} // namespace mln


#endif //! MLN_MORPHO_TOS_PRIVATE_IMMERSION_HPP
