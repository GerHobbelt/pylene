#pragma once
#include <mln/core/image/image.hpp>
#include <mln/core/range/rows.hpp>
#include <mln/core/range/view/zip.hpp>
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
  ///
  /// \tparam InputImage A model of :concept:ref:`forward_image`
  /// \tparam OutputImage A model of Writable :concept:ref:`forward_image`
  /// \tparam UnaryFunction A mondel of unary function.
  /// \param input The input image.
  /// \param f The unary function.
  /// \param output The output image.

  template <class InputImage, class OutputImage, class UnaryFunction>
  requires ::ranges::invocable<UnaryFunction, image_reference_t<InputImage>>
  void transform(InputImage in, OutputImage out, UnaryFunction f);

  template <class InputImage1, class InputImage2, class BinaryFunction>
  requires ::ranges::invocable<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>
  image_ch_value_t<InputImage1, std::decay_t<std::invoke_result_t<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>>>
  transform(InputImage1 in1, InputImage2 in2, BinaryFunction f);

  template <class InputImage1, class InputImage2, class OutputImage, class BinaryFunction>
  requires ::ranges::invocable<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>
  void transform(InputImage1 in1, InputImage2 in2, OutputImage out, BinaryFunction f);


  template <class InputImage, class UnaryFunction>
  image_ch_value_t<InputImage, std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>>
  transform(InputImage in, UnaryFunction f);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class InputImage, class OutputImage, class UnaryFunction>
  requires ::ranges::invocable<UnaryFunction, image_reference_t<InputImage>>
  void transform(InputImage in, OutputImage out, UnaryFunction f)
  {
    static_assert(mln::is_a<InputImage,  mln::details::Image>());
    static_assert(mln::is_a<OutputImage, mln::details::Image>());
    static_assert(std::is_convertible_v<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>,
                  image_value_t<OutputImage>>,
                  "The result of the function is not implicitely convertible to the output image value type");

    mln_entering("mln::transform");
    mln_precondition(in.domain() == out.domain());

    auto&& ivals = in.values();
    auto&& ovals = out.values();
    for (auto [r1, r2] : mln::ranges::view::zip(ranges::rows(ivals), ranges::rows(ovals)))
      ::ranges::transform(r1, ::ranges::begin(r2), f);
  }

  template <class InputImage1, class InputImage2, class BinaryFunction>
  requires ::ranges::invocable<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>
  image_ch_value_t<InputImage1, std::decay_t<std::invoke_result_t<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>>>
  transform(InputImage1 in1, InputImage2 in2, BinaryFunction f)
  {
    static_assert(mln::is_a<InputImage1, mln::details::Image>());
    static_assert(mln::is_a<InputImage2, mln::details::Image>());

    mln_entering("mln::transform");

    using R = std::decay_t<std::invoke_result_t<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>>;
    using O = image_ch_value_t<InputImage1, R>;

    // Check concretizable
    O out = imchvalue<R>(in1);
    mln::transform(in1, in2, out, f);
    return out;
  }

  template <class InputImage1, class InputImage2, class OutputImage, class BinaryFunction>
  requires ::ranges::invocable<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>
  void transform(InputImage1 in1, InputImage2 in2, OutputImage out, BinaryFunction f)
  {
    static_assert(mln::is_a<InputImage1, mln::details::Image>());
    static_assert(mln::is_a<InputImage2, mln::details::Image>());
    static_assert(mln::is_a<OutputImage, mln::details::Image>());
    static_assert(
        std::is_convertible_v<
            std::invoke_result_t<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>,
            image_value_t<OutputImage>>,
        "The result of the function is not implicitely convertible to the output image value type");

    mln_entering("mln::transform");

    // FIXME: disabled becaused some domain (e.g. morphed) do not implement comparison for now
    // mln_precondition(in2.domain() == out.domain());
    // mln_precondition(in1.domain() == out.domain());

    auto&& ivals1 = in1.values();
    auto&& ivals2 = in2.values();
    auto&& ovals  = out.values();
    for (auto [r1, r2, r3] : mln::ranges::view::zip(ranges::rows(ivals1), ranges::rows(ivals2), ranges::rows(ovals)))
      ::ranges::transform(r1, r2, ::ranges::begin(r3), f);
  }

  template <class InputImage, class UnaryFunction>
  requires ::ranges::invocable<UnaryFunction, image_reference_t<InputImage>>
  image_ch_value_t<InputImage, std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>>
  transform(InputImage in, UnaryFunction f)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>());

    using R = std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>;
    using O = image_ch_value_t<InputImage, R>;

    // Check concretizable
    O out = imchvalue<R>(in);
    mln::transform(in, out, f);
    return out;
  }

  namespace parallel
  {
    namespace details
    {
      template <class Function, class InputImage, class OutputImage>
      class TransformParallel : public ParallelCanvas2d
      {
        InputImage  _in;
        OutputImage _out;
        Function    _fun;

        static_assert(mln::is_a<InputImage, mln::details::Image>());
        static_assert(mln::is_a<OutputImage, mln::details::Image>());


        mln::box2d GetDomain() const final { return _in.domain(); }

        void ExecuteTile(mln::box2d b) const final
        {
          auto subimage_in  = _in.clip(b);
          auto subimage_out = _out.clip(b);
          mln::transform(subimage_in, subimage_out, _fun);
        }

      public:
        TransformParallel(InputImage input, OutputImage output, Function fun)
          : _in{input}
          , _out{output}
          , _fun{fun}
        {
        }

      };

      template <class Function, class InputImage1, class InputImage2, class OutputImage>
      class Transform_BinaryFunc_Parallel : public ParallelCanvas2d
      {
        InputImage1 _in1;
        InputImage2 _in2;
        OutputImage _out;
        Function    _fun;

        static_assert(mln::is_a<InputImage1, mln::details::Image>());
        static_assert(mln::is_a<InputImage2, mln::details::Image>());
        static_assert(mln::is_a<OutputImage, mln::details::Image>());


        // XXX Will probably cause problems if in1 and in2 don't have the same domain or shape
        mln::box2d GetDomain() const final { return _in1.domain(); }

        void ExecuteTile(mln::box2d b) const final
        {
          auto subimage_in1  = _in1.clip(b);
          auto subimage_in2  = _in2.clip(b);
          auto subimage_out = _out.clip(b);
          mln::transform(subimage_in1, subimage_in2, subimage_out, _fun);
        }

      public:
        Transform_BinaryFunc_Parallel(InputImage1 input, InputImage2 input2, OutputImage output, Function fun)
          : _in1{input}
          , _in2{input2}
          , _out{output}
          , _fun{fun}
        {
        }

      };
    } // namespace details

    template <class InputImage, class UnaryFunction>
    requires ::ranges::invocable<UnaryFunction, image_reference_t<InputImage>>
    image_ch_value_t<InputImage, std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>>
    transform(InputImage in, UnaryFunction f)
    {
      using R = std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>;
      using O = image_ch_value_t<InputImage, R>;

      // Check concretizable
      O out = imchvalue<R>(in);

      details::TransformParallel caller(in, out, f);
      parallel_execute2d(caller);

      return out;
    }

    template <class InputImage, class OutputImage, class UnaryFunction>
    requires ::ranges::invocable<UnaryFunction, image_reference_t<InputImage>>
    void transform(InputImage in, OutputImage out, UnaryFunction f)
    {
      details::TransformParallel caller(in, out, f);
      parallel_execute2d(caller);
    }

    template <class InputImage1, class InputImage2, class BinaryFunction>
    requires ::ranges::invocable<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>
    image_ch_value_t<InputImage1, std::decay_t<std::invoke_result_t<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>>>
    transform(InputImage1 in1, InputImage2 in2, BinaryFunction f)
    {
      assert(in1.domain() == in2.domain());
      using R = std::decay_t<std::invoke_result_t<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>>;
      using O = image_ch_value_t<InputImage1, R>;

      // Check concretizable
      O out = imchvalue<R>(in1);
      details::Transform_BinaryFunc_Parallel caller(in1, in2, out, f);
      parallel_execute2d(caller);

      return out;
    }

    template <class InputImage1, class InputImage2, class OutputImage, class BinaryFunction>
    requires ::ranges::invocable<BinaryFunction, image_reference_t<InputImage1>, image_reference_t<InputImage2>>
    void transform(InputImage1 in1, InputImage2 in2, OutputImage out, BinaryFunction f)
    {
      assert(in1.domain() == in2.domain());
      details::Transform_BinaryFunc_Parallel caller(in1, in2, out, f);
      parallel_execute2d(caller);
    }
  } // namespace parallel

} // namespace mln
