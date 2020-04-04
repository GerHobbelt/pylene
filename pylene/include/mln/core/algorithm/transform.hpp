#pragma once
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/assert.hpp>


#include <range/v3/algorithm/transform.hpp>
#include <range/v3/functional/concepts.hpp>
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

  template <class InputImage1, class InputImage2, class OutputImage, class BinaryFunction>
  void transform(InputImage1 in1, InputImage2 in2, OutputImage out, BinaryFunction f);



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
    static_assert(mln::is_a<OutputImage, experimental::Image>());
    static_assert(::ranges::cpp20::invocable<UnaryFunction, image_reference_t<InputImage>>);
    static_assert(std::is_convertible_v<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>,
                  image_value_t<OutputImage>>,
                  "The result of the function is not implicitely convertible to the output image value type");

    mln_entering("mln::transform");
    mln_precondition(in.domain() == out.domain());

    auto&& ivals = in.new_values();
    auto&& ovals = out.new_values();
    for (auto [r1, r2] : mln::ranges::view::zip(ranges::rows(ivals), ranges::rows(ovals)))
      ::ranges::transform(r1, ::ranges::begin(r2), f);
  }

  template <class InputImage1, class InputImage2, class OutputImage, class BinaryFunction>
  void transform(InputImage1 in1, InputImage2 in2, OutputImage out, BinaryFunction f)
  {
    static_assert(mln::is_a<InputImage1, experimental::Image>());
    static_assert(mln::is_a<InputImage2, experimental::Image>());
    static_assert(mln::is_a<OutputImage, experimental::Image>());
    static_assert(
        ::ranges::cpp20::invocable<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>);
    static_assert(
        std::is_convertible_v<
            std::invoke_result_t<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>,
            image_value_t<OutputImage>>,
        "The result of the function is not implicitely convertible to the output image value type");

    mln_entering("mln::transform");

    // FIXME: disabled becaused some domain (e.g. morphed) do not implement comparison for now
    // mln_precondition(in2.domain() == out.domain());
    // mln_precondition(in1.domain() == out.domain());

    auto&& ivals1 = in1.new_values();
    auto&& ivals2 = in2.new_values();
    auto&& ovals  = out.new_values();
    for (auto [r1, r2, r3] : mln::ranges::view::zip(ranges::rows(ivals1), ranges::rows(ivals2), ranges::rows(ovals)))
      ::ranges::transform(r1, ::ranges::begin(r2), ::ranges::begin(r3), f);
  }

  template <class InputImage, class UnaryFunction>
  image_ch_value_t<InputImage, std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>>
  transform(InputImage in, UnaryFunction f)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::cpp20::invocable<UnaryFunction, image_reference_t<InputImage>>);

    using R = std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>;
    using O = image_ch_value_t<InputImage, R>;

    // Check concretizable
    O out = in.template ch_value<R>();
    mln::transform(in, out, f);
    return out;
  }


} // namespace mln
