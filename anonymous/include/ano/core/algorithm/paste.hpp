#pragma once
#include <ano/core/algorithm/copy.hpp>
#include <ano/core/canvas/parallel_pointwise.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/range/foreach.hpp>
#include <ano/core/range/rows.hpp>

#include <ano/core/trace.hpp>

namespace ano
{

  /// \brief \p paste copy each pixel from the input image to the output image.
  ///
  /// \ingroup Algorithms
  ///
  /// Each pixel of the input image is paste to the output image. It implies
  /// that \p input's domain must be included in \p output's domain.

  /// This is equivalent to the following code:
  ///
  /// \code
  /// ano_foreach (auto px, input.pixels())
  ///    output(px.point()) = px.val()
  /// \endcode
  ///
  /// \param[in] input The input image.
  /// \param[out] output The output image
  ///
  /// \return The output image.
  ///
  /// \pre `output.domain()` includes `input.domain()`
  /// \tparam InputImage is a model of the Forward Image.
  /// \tparam OutputImage is a model of the Writable Point-Accessible Image
  template <class InputImage, class OutputImage>
  void paste(InputImage src, OutputImage dest);

  template <class InputImage, class InputRange, class OutputImage>
  void paste(InputImage src, InputRange roi, OutputImage dest);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    template <class I, class D = image_domain_t<I>, class = void>
    struct is_image_clippable : std::false_type
    {
    };

    template <class I, class D>
    struct is_image_clippable<I, D, std::void_t<decltype(std::declval<I>().clip(std::declval<D>()))>> : std::true_type
    {
    };

    template <class I, class D>
    inline constexpr bool is_image_clippable_v = is_image_clippable<I, D>::value;
  } // namespace details

  namespace impl
  {
    // Like paste but allows access to the extension
    template <class InputImage, class InputRange, class OutputImage>
    void paste_unsafe(InputImage src, InputRange roi, OutputImage dest)
    {
      ano_foreach (auto p, roi)
        dest.at(p) = src.at(p);
    }
  } // namespace impl


  template <class InputImage, class InputRange, class OutputImage>
  void paste(InputImage src, InputRange roi, OutputImage dest)
  {
    ano_entering("ano::paste");
    // FIXME: Add a precondition about the domain inclusion
    // FIXME: check OutputImage is accessible
    static_assert(ano::is_a<InputImage, ano::details::Image>());
    static_assert(ano::is_a<OutputImage, ano::details::Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    if constexpr (details::is_image_clippable_v<InputImage, InputRange> &&
                  details::is_image_clippable_v<OutputImage, InputRange>)
    {
      ano::copy(src.clip(roi), dest.clip(roi));
    }
    else
    {
      ano_foreach (auto p, roi)
        dest(p) = src(p);
    }
  }

  template <class InputImage, class OutputImage>
  void paste(InputImage src, OutputImage dest)
  {
    ano_entering("ano::paste");
    // FIXME: Add a precondition about the domain inclusion
    // FIXME: check OutputImage is accessible
    static_assert(ano::is_a<InputImage, ano::details::Image>());
    static_assert(ano::is_a<OutputImage, ano::details::Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    using InputRange = image_domain_t<InputImage>;

    if constexpr (details::is_image_clippable_v<OutputImage, InputRange>)
    {
      ano::copy(src, dest.clip(src.domain()));
    }
    else
    {
      auto&& pixels = src.pixels();
      for (auto row : ranges::rows(pixels))
        for (auto px : row)
          dest(px.point()) = px.val();
    }
  }

  namespace parallel
  {
    namespace details
    {
      template <class InputImage, class OutputImage>
      class PasteParallel : public ParallelCanvas2d
      {
        InputImage  _in;
        OutputImage _out;

        static_assert(ano::is_a<InputImage, ano::details::Image>());
        static_assert(ano::is_a<OutputImage, ano::details::Image>());
        static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

        void ExecuteTile(ano::box2d b) const final
        {
          auto subimage_in  = _in.clip(b);
          auto subimage_out = _out.clip(b);
          ano::paste(subimage_in, subimage_out);
        }

        ano::box2d GetDomain() const final { return _in.domain(); }

      public:
        PasteParallel(InputImage input, OutputImage output)
          : _in{input}
          , _out{output}
        {
        }
      };
    } // namespace details

    template <class InputImage, class OutputImage>
    void paste(InputImage src, OutputImage dest)
    {
      details::PasteParallel caller(src, dest);
      parallel_execute2d(caller);
    }
  } // namespace parallel

} // namespace ano
