#pragma once

#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/neighborhood.hpp>

#include <ano/core/algorithm/clone.hpp>
#include <ano/morpho/experimental/canvas/unionfind.hpp>


namespace ano::morpho::experimental
{

  /// \brief Compute the dynamic algebraic opening of an image.
  ///
  /// Also called h-maxima transformation. The dynamic represents the difference between the maximum and the minimum of
  /// the connected component.
  ///
  /// \param ima The input image
  /// \param nbh The neighborhood
  /// \param area The grain size
  /// \param cmp A strict total ordering on values
  template <class I, class N, class Compare = std::less<image_value_t<std::remove_reference_t<I>>>>
  image_concrete_t<std::remove_reference_t<I>> //
  dynamic_opening(I&& ima, const N& nbh, int dynamic, Compare cmp = {});

  /// \brief Compute the dynamic algebraic closing of an image.
  ///
  /// Also called h-minima transformation. The dynamic represents the difference between the maximum and the minimum of
  /// the connected component.
  ///
  /// \param ima The input image
  /// \param nbh The neighborhood
  /// \param area The grain size
  template <class I, class N>
  image_concrete_t<std::remove_reference_t<I>> //
  dynamic_closing(I&& ima, const N& nbh, int dynamic);

  /******************************/
  /*** Implementation          **/
  /******************************/

  namespace impl
  {
    template <class I, class Compare>
    struct dynamic_filter_ufind_visitor
    {
      using P = image_point_t<I>;

      void on_make_set(P p) noexcept { m_max(p) = m_ima(p); }

      void on_finish(P p, P root) noexcept { m_ima(p) = m_ima(root); }

      bool test(P p) const noexcept { return std::abs(m_max(p) - m_ima(p)) >= m_dynamic; }

      P on_union(dontcare_t, P rp, dontcare_t, P rq) noexcept
      {
        auto a    = m_max(rp);
        auto b    = m_max(rq);
        m_max(rp) = m_cmp(a, b) ? b : a;
        return rp;
      }

      dynamic_filter_ufind_visitor(I& input, Compare cmp, int dynamic)
        : m_ima(input)
        , m_cmp(cmp)
        , m_dynamic(dynamic)
      {
        m_max = imconcretize(input);
      }


    private:
      I&                  m_ima;
      Compare             m_cmp;
      int                 m_dynamic;
      image_concrete_t<I> m_max;
    };


    template <class I, class N, class Compare>
    void dynamic_opening_inplace(I& ima, const N& nbh, int dynamic, Compare cmp)
    {
      dynamic_filter_ufind_visitor viz(ima, cmp, dynamic);
      ano::morpho::experimental::canvas::union_find(ima, nbh, viz, cmp);
    }

  } // namespace impl


  template <class InputImage, class N, class Compare>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  dynamic_opening(InputImage&& ima, const N& nbh, int dynamic, Compare cmp)
  {
    using I = std::remove_reference_t<InputImage>;

    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(ano::is_a<N, ano::details::Neighborhood>());

    ano_entering("ano::morpho::dynamic_opening");
    image_concrete_t<I> out = clone(ima);
    impl::dynamic_opening_inplace(out, nbh, dynamic, std::move(cmp));

    return out;
  }

  template <class InputImage, class N>
  image_concrete_t<std::remove_reference_t<InputImage>> //
  dynamic_closing(InputImage&& ima, const N& nbh, int dynamic)
  {
    using I = std::remove_reference_t<InputImage>;

    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(ano::is_a<N, ano::details::Neighborhood>());

    ano_entering("ano::morpho::dynamic_closing");
    image_concrete_t<I> out = clone(ima);
    impl::dynamic_opening_inplace(out, nbh, dynamic, std::greater<image_value_t<I>>());

    return out;
  }

} // namespace ano::morpho::experimental
