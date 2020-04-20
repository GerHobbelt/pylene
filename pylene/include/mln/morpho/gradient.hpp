#pragma once

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/concepts/structuring_element.hpp>

#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>
#include <mln/core/private/maths_ops.hpp>

/// \file

namespace mln::morpho
{
  /// \ingroup morpho
  /// \brief Compute the Beucher gradient.
  ///
  /// The beucher gradient is defined as:
  /// \f[
  ///  |\nabla u(p)| = \left| \bigvee_{q \in \mathcal{N}(p)} f(q) -
  ///    \bigwedge_{q \in \mathcal{N}(p)} f(q) \right|
  /// \f]
  ///
  /// + If the optional \p out image is provided, it must be wide enough to store
  ///   the results (the function does not perform any resizing).
  ///
  /// + If a optional \p cmp function is provided, the algorithm
  ///   will internally do an unqualified call to `sup(x, y, cmp)`
  ///   and `inf(x, y,cmp)`.  The default is the product-order so
  ///   that it works for vectorial type as well.
  ///
  /// \param[in] input        Input image.
  /// \param[in] se   Neighborhood/SE/Window to look around.
  /// \param[in] cmp  (optional) Comparaison function. The method internally does an
  ///                 unqualified call to `inf(x, y, cmp)` and `sup(x, y, cmp)`. Default
  ///                 is the product-order.
  /// \param[in] norm (optional) Norm function used in \f$|x - y|\f$. The default is
  ///                  the ℓ₂-norm.
  /// \param[out] out (optional) Output image to write in.
  ///
  /// FIXME: This can be done in a single pass using kernels but you
  /// we have to consider:
  /// * an incrementable accumlator
  /// * an extension that is mirrorizable

  namespace details
  {
    template <class T, class enable = void>
    struct grad_op
    {
      using result_type = decltype(l2norm(std::declval<T>()));

      result_type operator()(T a, T b) const noexcept { return l2norm(T(a - b)); }
    };

    template <class T>
    struct grad_op<T, std::enable_if_t<std::is_arithmetic_v<T>>>
    {
      using result_type = T;

      T operator()(T a, T b) const noexcept { return a - b; }
    };

    template <class I>
    using gradient_result_t = image_ch_value_t<I, typename grad_op<image_value_t<I>>::result_type>;
  }


  template <class InputImage, class SE>
  details::gradient_result_t<std::remove_reference_t<InputImage>> gradient(InputImage&& input, const SE& se);

  template <class InputImage, class SE>
  details::gradient_result_t<std::remove_reference_t<InputImage>> external_gradient(InputImage&& input, const SE& se);

  template <class InputImage, class SE>
  details::gradient_result_t<std::remove_reference_t<InputImage>> internal_gradient(InputImage&& input, const SE& se);

  /*************************/
  /***  Implementation   ***/
  /*************************/

  template <class InputImage, class SE>
  details::gradient_result_t<std::remove_reference_t<InputImage>> gradient(InputImage&& ima, const SE& se)
  {
    using I = std::remove_reference_t<InputImage>;
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<SE, mln::details::StructuringElement>());

    mln_entering("mln::morpho::gradient");

    auto d = morpho::dilation(ima, se);
    auto e = morpho::erosion(ima, se);

    using R = typename details::grad_op<image_value_t<I>>::result_type;

    details::gradient_result_t<I> out = imchvalue<R>(ima);
    mln::transform(d, e, out, details::grad_op<image_value_t<I>>());
    return out;
  }

  template <class InputImage, class SE>
  details::gradient_result_t<std::remove_reference_t<InputImage>> external_gradient(InputImage&& ima, const SE& se)
  {
    using I = std::remove_reference_t<InputImage>;
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<SE, mln::details::StructuringElement>());

    mln_entering("mln::morpho::external_gradient");
    auto d = morpho::dilation(ima, se);

    using R = typename details::grad_op<image_value_t<I>>::result_type;

    details::gradient_result_t<I> out = imchvalue<R>(ima);
    mln::transform(d, ima, out, details::grad_op<image_value_t<I>>());
    return out;
  }

  template <class InputImage, class SE>
  details::gradient_result_t<std::remove_reference_t<InputImage>> internal_gradient(InputImage&& ima, const SE& se)
  {
    using I = std::remove_reference_t<InputImage>;
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<SE, mln::details::StructuringElement>());

    mln_entering("mln::morpho::internal_gradient");
    auto e = morpho::erosion(ima, se);

    using R = typename details::grad_op<image_value_t<I>>::result_type;

    details::gradient_result_t<I> out = imchvalue<R>(ima);
    mln::transform(ima, e, out, details::grad_op<image_value_t<I>>());
    return out;
  }

} // namespace mln::morpho::
