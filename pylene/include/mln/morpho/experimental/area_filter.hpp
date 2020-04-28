#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/neighborhood.hpp>

#include <mln/core/algorithm/clone.hpp>
#include <mln/morpho/experimental/canvas/unionfind.hpp>



namespace mln::morpho::experimental
{

  /// \brief Compute the area algebraic opening of an image.
  /// \param ima The input image
  /// \param nbh The neighborhood
  /// \param area The grain size
  /// \param cmp A strict total ordering on values
  template <class I, class N, class Compare = std::less<image_value_t<std::remove_reference_t<I>>>>
  image_concrete_t<std::remove_reference_t<I>> //
  area_opening(I&& ima, const N& nbh, int area, Compare cmp = {});

  /// \brief Compute the area algebraic closing of an image.
  /// \param ima The input image
  /// \param nbh The neighborhood
  /// \param area The grain size
  template <class I, class N>
  image_concrete_t<std::remove_reference_t<I>> //
  area_closing(I&& ima, const N& nbh, int area);

  /******************************/
  /*** Implementation          **/
  /******************************/

  namespace impl
  {
    template <class I>
    struct area_filter_ufind_visitor
    {
      using P = image_point_t<I>;

      void on_make_set(P p) noexcept { m_count(p) = 1; }

      P on_union(dontcare_t, P rp, dontcare_t, P rq) noexcept
      {
        m_count(rp) += m_count(rq);
        return rp;
      }

      void on_finish(P p, P root) noexcept { m_ima(p) = m_ima(root); }

      bool test(P p) const noexcept { return m_count(p) >= m_area; }

      area_filter_ufind_visitor(I& input, int area)
        : m_ima(input)
        , m_area(area)
      {
        m_count = imchvalue<int>(input);
      }


    private:
      I&                       m_ima;
      int                      m_area;
      image_ch_value_t<I, int> m_count;
    };


    template <class I, class N, class Compare>
    void area_opening_inplace(I& ima, const N& nbh, int area, Compare cmp)
    {
      area_filter_ufind_visitor<I> viz(ima, area);
      mln::morpho::experimental::canvas::union_find(ima, nbh, viz, cmp);
    }

  }


  template <class InputImage, class N, class Compare>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  area_opening(InputImage&& ima, const N& nbh, int area, Compare cmp)
  {
    using I = std::remove_reference_t<InputImage>;

    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<N, mln::experimental::Neighborhood>());

    mln_entering("mln::morpho::area_opening");
    image_concrete_t<I> out = clone(ima);
    impl::area_opening_inplace(out, nbh, area, std::move(cmp));

    return out;
  }

  template <class InputImage, class N>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  area_closing(InputImage&& ima, const N& nbh, int area)
  {
    using I = std::remove_reference_t<InputImage>;

    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<N, mln::experimental::Neighborhood>());

    mln_entering("mln::morpho::area_closing");
    image_concrete_t<I> out = clone(ima);
    impl::area_opening_inplace(out, nbh, area, std::greater<image_value_t<I>>());

    return out;
  }

} // namespace mln::moprho::experimental
