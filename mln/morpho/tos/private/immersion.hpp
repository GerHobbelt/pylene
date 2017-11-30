#ifndef MLN_MORPHO_TOS_PRIVATE_IMMERSION_HPP
# define MLN_MORPHO_TOS_PRIVATE_IMMERSION_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/image2d.hpp>
# include <mln/core/trace.hpp>
# include <mln/morpho/tos/irange.hpp>


namespace mln
{
  namespace morpho
  {
    namespace tos
    {
      namespace impl
      {

        /// \brief Immerse a 2d or 3d image into twice-as-big 2d image
        /// where the in-between pixel are intervals.
        ///
        /// Given 4 pixels:
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
	mln_ch_value(I, irange<mln_value(I)>)
        immersion(const Image<I>& ima);


        /******************************************/
        /****          Implementation          ****/
        /******************************************/


        template <int dim, class O>
        std::enable_if_t<(dim < (O::ndim - 1))>
        interpolation(O& output, mln_point(O) pmin, mln_point(O) pmax)
        {
          using P = mln_point(O);
          mln::box<typename P::value_type, P::ndim> dom = {pmin, pmax};
          mln_foreach(auto p, dom)
            {
              auto prev = p;
              auto next = p;
              prev[dim]--;
              next[dim]++;
              output(p) = output(prev).extend(output(next));
            }
        }

        template <int dim, class O>
        std::enable_if_t<dim == (O::ndim - 1)>
        interpolation(O& output, mln_point(O) p, mln_point(O))
        {
          auto prev = p;
          auto next = p;
          prev[dim]--;
          next[dim]++;
          output(p) = output(prev).extend(output(next));
        }

        template <int dim, class I, class O>
        std::enable_if_t<dim == O::ndim>
        immersion(const I& input, O& output, mln_point(I) pmin, mln_point(I))
        {
          output(pmin) = input(pmin / 2);
        }

        template <int dim, class I, class O>
        std::enable_if_t<(dim < O::ndim)>
        immersion(const I& input, O& output, mln_point(I) pmin, mln_point(I) pmax)
        {
          immersion<dim+1>(input, output, pmin, pmax);

          for (int i = pmin[dim] + 2, end = pmax[dim]; i < end; i += 2)
            {
              pmin[dim] = i;
              pmax[dim] = (i+1);
              immersion<dim+1>(input, output, pmin, pmax);

              pmin[dim] = (i-1);
              pmax[dim] = i;
              interpolation<dim>(output, pmin, pmax);
            }
        }

        template <class I>
	mln_ch_value(I, irange<mln_value(I)>)
        immersion(const Image<I>& ima_)
        {
          static_assert(image_traits<I>::accessible::value,
                        "Image must be accessible i.e. 'ima(p)'");

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



        /*
        template <class I>
        mln_ch_value(I, irange<mln_value(I)>)
        immersion_impl(const Image<I>& ima_, box3d)
        {
          static_assert(std::is_same<typename I::domain_type, box3d>::value, "Only box3d handled");
          static_assert(image_traits<I>::accessible::value,
                        "Image must be accessible i.e. 'ima(p)'");

          mln_entering("mln::morpho::tos - immersion");

          using V = mln_value(I);
          using R = irange<V>;
          using P = point2d;

	  const I& ima = exact(ima_);


          box2d output_domain = ima.domain();
	  output_domain.pmin = output_domain.pmin * 2;
	  output_domain.pmax = output_domain.pmax * 2 - 1;

	  image2d<R> out(output_domain);

          // Remove last line/col of the input domain
          box2d reduced_input_domain = ima.domain();
          reduced_input_domain.pmax -= 1;

	  mln_foreach(point2d p, reduced_input_domain)
          {
            V a = ima(p);
            V b = ima(p + P{0, 1});
            V b = ima(p + P{0, 1});
            V c = ima(p + P{1, 0});
            V d = ima(p + P{1, 1});

            V min_AB, max_AB, min_AC, max_AC;

            std::tie(min_AB, max_AB) = std::minmax(a,b);
            std::tie(min_AC, max_AC) = std::minmax(a,c);
            V min_all = std::min(std::min(min_AB, min_AC), d);
            V max_all = std::max(std::max(max_AB, max_AC), d);

            point2d q = 2 * p;
            out(q) = a;
            out(q + P{0, 1}) = R{min_AB, max_AB};
            out(q + P{1, 0}) = R{min_AC, max_AC};
            out(q + P{1, 1}) = R{min_all, max_all};
          }

          // handle last line / column
          {
            point2d pmin = ima.domain().pmin;
            point2d pmax = ima.domain().pmax;

            short x = pmax[1] - 1;
            short y = pmax[0] - 1;
            out.at(2 * y, 2 * x) = ima(P{y, x});

            for (short y = pmin[0]; y < pmax[0] - 1; ++y)
            {
              V a = ima(P{y, x}), b = ima(P{static_cast<short>(y + 1), x});
              out.at(2 * y, 2 * x) = a;
              out.at(2 * y + 1, 2 * x) = R{std::min(a, b), std::max(a, b)};
            }

            for (short x = pmin[1]; x < pmax[1] - 1; ++x)
            {
              V a = ima(P{y, x}), b = ima(P{y, static_cast<short>(x + 1)});
              out.at(2 * y, 2 * x) = a;
              out.at(2 * y, 2 * x + 1) = R{std::min(a, b), std::max(a, b)};
            }
          }

          return out;
        }
        */

      } // end of namespace mln::morpho::tos::impl
    } // end of namespace mln::morpho::tos
  } // end of namespace mln::morpho
} // end of namespace mln



#endif //!MLN_MORPHO_TOS_PRIVATE_IMMERSION_HPP
