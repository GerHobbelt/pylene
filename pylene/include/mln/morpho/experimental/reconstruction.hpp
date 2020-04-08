#pragma once


#include <mln/core/concept/images.hpp>
#include <mln/core/concept/neighborhoods.hpp>

#include <mln/morpho/experimental/canvas/unionfind.hpp>


namespace mln::morpho::experimental
{

  template <class I, class J, class N, class Compare = std::less<image_value_t<I>>>
  image_concrete_t<I> opening_by_reconstruction(I f, J markers, N nbh, Compare cmp = Compare());

  template <class I, class J, class N>
  image_concrete_t<I> closing_by_reconstruction(I f, J markers, N nbh);

  /*************************/
  /** Implementation     ***/
  /*************************/

  namespace detail
  {

    template <class I, class J, class O, class Compare>
    struct opening_by_rec_ufind_visitor
    {
      using P = image_point_t<I>;

      void on_make_set(P p) { m_out(p) = m_markers(p); }

      P on_union(dontcare_t, P rp, dontcare_t, P rq)
      {
        auto a = m_out(rp);
        auto b = m_out(rq);
        m_out(rp) = m_cmp(a, b) ? b : a;
        return rp;
      }

      void on_finish(P p, P q) { m_out(p) = m_input(q); }

      bool test(P p) { return !m_cmp(m_out(p), m_input(p)); }

      I       m_input;
      J       m_markers;
      O       m_out;
      Compare m_cmp;
    };
  } // namespace detail

  template <class I, class J, class N, class Compare>
  image_concrete_t<I>
  opening_by_reconstruction(I f, J markers, N nbh, Compare cmp)
  {
    static_assert(mln::is_a<I, mln::experimental::Image>::value);
    static_assert(mln::is_a<J, mln::experimental::Image>::value);
    static_assert(mln::is_a<N, mln::experimental::Neighborhood>::value);

    static_assert(std::is_convertible_v<image_value_t<J>, image_value_t<I>>,
                  "Marker image value type must be convertible to f's value type.");

    static_assert(std::is_same_v<image_domain_t<J>, image_domain_t<I>>,
                  "Images f and markers must be defined over the same domain.");

    mln_entering("mln::morpho::opening_by_reconstruction");

    // assert that f <= markers
    // FIXME:
    // mln_precondition(all_of(view::transform(view::zip(f, markers), [cmp](std::tuple<mln_value(I), mln_value(J)> v)
    // {
    //   return not cmp(std::get<1>(v), std::get<0>(v));
    // })));

    image_concrete_t<I> out = imconcretize(f);
    detail::opening_by_rec_ufind_visitor<I, J, image_concrete_t<I>, Compare> viz = {f, markers, out, cmp};
    mln::morpho::experimental::canvas::union_find(f, nbh, viz, cmp);
    return out;
  }


  template <class I, class J, class N>
  image_concrete_t<I>
  closing_by_reconstruction(I f, J markers, N nbh)
  {
    return opening_by_reconstruction(std::move(f), std::move(markers), std::move(nbh), std::greater<image_value_t<I>>());
  }

} // namespace mln::morpho::experimental
