#pragma once
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/trace.hpp>

#include <range/v3/algorithm/transform.hpp>
#include <range/v3/utility/functional.hpp>
/// \file

namespace mln
{

  /// \ingroup Algorithms
  /// \brief Transform the value of an image through a function.
  ///
  /// This is equivalent to the following code.
  /// \code
  /// mln_iter(vout, out.values())
  /// mln_iter(vin, in.values())
  /// mln_forall(vin, vout)
  ///    *vout = f(*vin)
  ///
  /// \endcode
  ///
  /// \tparam InputImage A model of :concept:ref:`forward_image`
  /// \tparam OutputImage A model of Writable :concept:ref:`forward_image`
  /// \tparam UnaryFunction A mondel of unary function.
  /// \param input The input image.
  /// \param f The unary function.
  /// \param output The output image.

  template <class InputImage, class OutputImage, class UnaryFunction>
  void transform(InputImage in, const Image<OutputImage>& out, UnaryFunction f);


  template <class InputImage, class UnaryFunction>
  image_ch_value_t<InputImage, std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>>
      transform(InputImage in, UnaryFunction f);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class InputImage, class OutputImage, class UnaryFunction>
  void transform(InputImage in, OutputImage out, UnaryFunction f)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::Invocable<UnaryFunction, image_reference_t<InputImage>>());
    static_assert(std::is_convertible_v<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>,
                                        image_value_t<OutputImage>>,
                  "The result of the function is not implicitely convertible to the output image value type");

    mln_entering("mln::transform");
    mln_precondition(in.domain() == out.domain());

    auto&& ivals = in.new_values();
    auto&& ovals = out.new_values();
    for (auto [r1, r2] : ranges::view::zip(ranges::rows(ivals), ranges::rows(ovals)))
      ::ranges::transform(r1, ::ranges::begin(r2), f);
  }

  template <class InputImage, class UnaryFunction>
  image_ch_value_t<InputImage, std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>>
      transform(InputImage in, UnaryFunction f)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::Invocable<UnaryFunction, image_reference_t<InputImage>>());

    using R = std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>;
    using O = image_ch_value_t<InputImage, R>;

    // Check concretizable
    O out = in.template ch_value<R>();
    mln::transform(in, out, f);
    return out;
  }


} // namespace mln
