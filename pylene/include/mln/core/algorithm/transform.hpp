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
  /// \see mln::imtransform
  template <typename InputImage, typename OutputImage, class UnaryFunction>
  [[deprecated]] OutputImage& transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>& output);

  /// \overload
  template <typename InputImage, typename OutputImage, class UnaryFunction>
  [[deprecated]] OutputImage&& transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>&& output);

  template <typename InputImage, class UnaryFunction>
  [[deprecated]] unary_image_expr<UnaryFunction, InputImage> lazy_transform(InputImage&& input, UnaryFunction f);

  /// \overload
  template <typename InputImage, class UnaryFunction>
  [[deprecated]] mln_ch_value(
      InputImage, typename std::decay<typename std::result_of<UnaryFunction(mln_value(InputImage))>::type>::type)
      transform(const Image<InputImage>& input, UnaryFunction f);


  namespace experimental
  {
    template <class InputImage, class OutputImage, class UnaryFunction>
    void transform(InputImage in, const Image<OutputImage>& out, UnaryFunction f);


    template <class InputImage, class UnaryFunction>
    image_ch_value_t<InputImage, std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>>
        transform(InputImage in, UnaryFunction f);
  } // namespace experimental

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {
    template <typename I, typename J, class UnaryFunction>
    void transform(const I& input, UnaryFunction f, J& output)
    {
      mln_viter(vin, vout, input, output);
      mln_forall (vin, vout)
        *vout = f(*vin);
    }
  } // namespace impl

  template <typename InputImage, typename OutputImage, class UnaryFunction>
  OutputImage& transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>& output)
  {
    OutputImage& out = exact(output);
    mln_entering("mln::transform");
    impl::transform(exact(input), f, exact(output));
    mln_exiting();
    return out;
  }

  template <typename InputImage, typename OutputImage, class UnaryFunction>
  OutputImage&& transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>&& output)
  {
    mln::transform(input, f, output);
    return move_exact(output);
  }

  template <typename InputImage, class UnaryFunction>
  mln_ch_value(InputImage,
               typename std::decay<typename std::result_of<UnaryFunction(mln_value(InputImage))>::type>::type)
      transform(const Image<InputImage>& input, UnaryFunction f)
  {
    typedef typename std::decay<typename std::result_of<UnaryFunction(mln_value(InputImage))>::type>::type T;

    mln_ch_value(InputImage, T) out = imchvalue<T>(exact(input));
    mln::transform(input, f, out);
    return out;
  }

  template <typename InputImage, class UnaryFunction>
  unary_image_expr<UnaryFunction, InputImage> lazy_transform(InputImage&& input, UnaryFunction f)
  {
    return make_unary_image_expr(std::forward<InputImage>(input), f);
  }


  namespace experimental
  {
    template <class InputImage, class OutputImage, class UnaryFunction>
    void transform(InputImage in, OutputImage out, UnaryFunction f)
    {
      static_assert(mln::is_a<InputImage, Image>());
      static_assert(::ranges::Invocable<UnaryFunction, image_reference_t<InputImage>>());
      static_assert(std::is_convertible_v<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>,
                                          image_value_t<OutputImage>>,
                    "The result of the function is not implicitely convertible to the output image value type");

      mln_entering("mln::experimental::transform");
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
      static_assert(mln::is_a<InputImage, Image>());
      static_assert(::ranges::Invocable<UnaryFunction, image_reference_t<InputImage>>());

      using R = std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>;
      using O = image_ch_value_t<InputImage, R>;

      // Check concretizable
      O out = in.template ch_value<R>();
      mln::experimental::transform(in, out, f);
      return out;
    }
  } // namespace experimental

} // namespace mln
