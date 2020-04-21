#pragma once

#include <mln/core/canvas/parallel_pointwise.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/range/rows.hpp>
#include <mln/core/range/view/zip.hpp>
#include <mln/core/trace.hpp>

#include <range/v3/algorithm/copy.hpp>

#include <algorithm>

/// \file

namespace mln::experimental
{

  /// \brief Copy the values of a source image to a destination image regardless their domain.
  ///
  /// \ingroup Algorithms
  ///
  /// The input and output image must have domain of the same size.
  /// This is equivalent to the following code.
  ///
  /// \code
  /// mln_foreach((auto&& [vin, vout]), ranges::zip(input.values(), output.values()))
  ///   *vout = *vin;
  /// \endcode
  ///
  /// \param[in] src Input Image
  /// \param[out] dest Output Image
  /// \return The image where values have been copied in.
  ///
  ///
  template <class InputImage, class OutputImage>
  void copy(InputImage src, OutputImage dest);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class InputImage, class OutputImage>
  void copy(InputImage input, OutputImage output)
  {
    mln_entering("mln::copy");

    // FIXME: Add a precondition about the size of the domain ::ranges::size
    static_assert(mln::is_a<InputImage, Image>());
    static_assert(mln::is_a<OutputImage, Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    auto&& ivals = input.new_values();
    auto&& ovals = output.new_values();

    for (auto [r1, r2] : ranges::view::zip(ranges::rows(ivals), ranges::rows(ovals)))
      ::ranges::copy(r1, ::ranges::begin(r2));
  }


  namespace parallel
  {
    namespace details
    {
      template <class InputImage, class OutputImage>
      class CopyParallel : public ParallelCanvas2d
      {
        InputImage  _in;
        OutputImage _out;

        static_assert(mln::is_a<InputImage, experimental::Image>());
        static_assert(mln::is_a<OutputImage, experimental::Image>());
        static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

        CopyParallel(InputImage input, OutputImage output)
          : _in{input}
          , _out{output}
        {
        }

        mln::experimental::box2d GetDomain() const final { return _in.domain(); }

      public:
        void ExecuteTile(mln::experimental::box2d b) const final
        {
          auto subimage_in  = _in.clip(b);
          auto subimage_out = _out.clip(b);
          mln::experimental::copy(subimage_in, subimage_out);
        }
      };
    } // namespace details

    template <class InputImage, class OutputImage>
    void copy(InputImage in, OutputImage out)
    {
      details::CopyParallel caller(in, out);
      parallel_execute2d(caller);
    }
  } // namespace parallel

} // namespace mln::experimental
