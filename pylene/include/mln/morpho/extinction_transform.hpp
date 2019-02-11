#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>
#include <mln/core/trace.hpp>

#include <mln/morpho/canvas/unionfind.hpp>



namespace mln::morpho
{

  /// \brief Compute the dynamic algebraic opening of an image.
  ///
  /// Also called h-maxima transformation
  ///
  /// \param ima The input image
  /// \param nbh The neighborhood
  /// \param area The grain size
  /// \param cmp A strict total ordering on values
  template <class I, class N>
  image_concrete_t<std::remove_reference_t<I>> //
  maxima_extinction_transform(I&& ima, const N& nbh);

  template <class I, class N>
  image_concrete_t<std::remove_reference_t<I>> //
  minima_extinction_transform(I&& ima, const N& nbh);



  /******************************/
  /*** Implementation          **/
  /******************************/

  namespace impl
  {
    template <class I, class O, class Compare>
    struct extinction_tranform_ufind_visitor
    {
      using P = image_point_t<I>;

      void on_make_set(P p) noexcept { m_output(p) = m_ima(p); }

      void on_finish(P p, dontcare_t) noexcept
        {
          m_output(p) = std::abs(m_ima(p) - m_output(p));
        }

      constexpr bool test(dontcare_t) { return false; }

      P on_union(P p, P rp, dontcare_t, P rq) noexcept
      {
        // Must ensure stability for extrema with flat zones
        // i.e. prefer rooting in p

        m_output(rp) = m_ima(p);
        m_output(rq) = m_ima(p);

        return m_cmp(m_ima(rp), m_ima(rq)) ? rq : rp;
      }

      extinction_tranform_ufind_visitor(I& input, O& output, Compare cmp)
        : m_ima(input)
        , m_output(output)
        , m_cmp(cmp)
      {
      }


    private:
      I&                       m_ima;
      O&                       m_output;
      Compare                  m_cmp;
    };


    template <class I, class N, class Compare>
    image_concrete_t<I> maxima_extinction_transform(I& input, const N& nbh, Compare cmp)
    {
      mln_entering("mln::morpho::maxima_extinction_transform");
      image_concrete_t<I> output = imconcretize(input);

      extinction_tranform_ufind_visitor viz(input, output, cmp);
      mln::morpho::canvas::union_find(input, nbh, viz, cmp);
      return output;
    }

  }


  template <class InputImage, class N>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  maxima_extinction_transform(InputImage&& ima, const N& nbh)
  {
    using I = std::remove_reference_t<InputImage>;

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());

    return impl::maxima_extinction_transform(ima, nbh, std::less<image_value_t<I>>());
  }

  template <class InputImage, class N>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  minima_extinction_transform(InputImage&& ima, const N& nbh)
  {
    using I = std::remove_reference_t<InputImage>;

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());

    return impl::maxima_extinction_transform(ima, nbh, std::greater<image_value_t<I>>());
  }

} // namespace mln::moprho::

