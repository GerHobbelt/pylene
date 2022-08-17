#pragma once

#include <ano/core/canvas/parallel_pointwise.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/range/rows.hpp>
#include <ano/core/range/view/zip.hpp>
#include <ano/core/trace.hpp>

#include <range/v3/algorithm/copy.hpp>

#include <algorithm>

/// \file

namespace ano
{

  /// \brief Copy the values of a source image to a destination image regardless their domain.
  ///
  /// \ingroup Algorithms
  ///
  /// The input and output image must have domain of the same size.
  /// This is equivalent to the following code.
  ///
  /// \code
  /// ano_foreach((auto&& [vin, vout]), ranges::zip(input.values(), output.values()))
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
    ano_entering("ano::copy");

    // FIXME: Add a precondition about the size of the domain ::ranges::size
    static_assert(ano::is_a<InputImage, ano::details::Image>());
    static_assert(ano::is_a<OutputImage, ano::details::Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    auto&& ivals = input.values();
    auto&& ovals = output.values();

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

        static_assert(ano::is_a<InputImage, ano::details::Image>());
        static_assert(ano::is_a<OutputImage, ano::details::Image>());
        static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);


        ano::box2d GetDomain() const final { return _in.domain(); }

        void ExecuteTile(ano::box2d b) const final
        {
          auto subimage_in  = _in.clip(b);
          auto subimage_out = _out.clip(b);
          ano::copy(subimage_in, subimage_out);
        }

      public:
        CopyParallel(InputImage input, OutputImage output)
          : _in{input}
          , _out{output}
        {
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

} // namespace ano
