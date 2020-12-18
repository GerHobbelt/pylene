#include <mln/core/algorithm/transform.hpp>
#include <mln/core/canvas/morpho_pipeline.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/morpho/closing.hpp>
#include <mln/morpho/opening.hpp>

namespace mln::morpho
{
  /// \brief Computes the bottom hat of a given image.
  ///
  /// The structurual morphological bottom hat γ of \p f by a structuring
  /// element 𝓑
  /// \f[
  /// \gamma(f, \mathcal{B}) = f - \varepsilon_\mathcal{B}(\delta_\mathcal{B}(f))
  /// \f]
  /// where ε denotes the erosion and δ the dilation.
  /// In other words, it can be expressed as
  /// \f[
  /// \gamma(f, \mathcal{B}) = f - \text{closing}(f, \mathcal{B})
  /// \f]
  ///
  ///
  /// + If the optional \p out image is provided, it must be wide enough to store
  ///   the results (the function does not perform any resizing).
  ///
  /// \param[in] input Input image \p f
  /// \param[in] se Structuring element/Neighborhood/Window \p B to look around.
  /// \param[out] out (optional) Output image \p g to write in.
  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> bottom_hat(InputImage&& image, const SE& se);

  template <class InputImage, class SE, class OutputImage>
  requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
  void bottom_hat(InputImage&& image, const SE& se, OutputImage&& out);


  /// \brief Computes the top hat of a given image.
  ///
  /// The structurual morphological top hat γ of \p f by a structuring
  /// element 𝓑
  /// \f[
  /// \gamma(f, \mathcal{B}) = \delta_\mathcal{B}(\varepsilon_\mathcal{B}(f)) - f
  /// \f]
  /// where ε denotes the erosion and δ the dilation.
  /// In other words, it can be expressed as
  /// \f[
  /// \gamma(f, \mathcal{B}) = \text{opening}(f, \mathcal{B}) - f
  /// \f]
  ///
  ///
  /// + If the optional \p out image is provided, it must be wide enough to store
  ///   the results (the function does not perform any resizing).
  ///
  /// \param[in] input Input image \p f
  /// \param[in] se Structuring element/Neighborhood/Window \p B to look around.
  /// \param[out] out (optional) Output image \p g to write in.
  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> top_hat(InputImage&& image, const SE& se);

  template <class InputImage, class SE, class OutputImage>
  requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
  void top_hat(InputImage&& image, const SE& se, OutputImage&& out);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> bottom_hat(InputImage&& image, const SE& se)
  {
    using I    = std::remove_reference_t<InputImage>;
    using Func = details::grad_op<image_value_t<I>>;
    using R    = std::invoke_result_t<Func, image_reference_t<I>, image_reference_t<I>>;
    using O    = image_ch_value_t<I, R>;

    auto closing = mln::morpho::closing(image, se);
    auto out     = mln::transform(image, closing, mln::morpho::details::grad_op<image_value_t<I>>());

    return static_cast<O&>(out);
  }

  template <class InputImage, class SE, class OutputImage>
  requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
  void bottom_hat(InputImage&& image, const SE& se, OutputImage&& out)
  {
    using I    = std::remove_reference_t<InputImage>;

    auto closing = mln::morpho::closing(image, se);
    mln::transform(image, closing, out, mln::morpho::details::grad_op<image_value_t<I>>());
  }

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> top_hat(InputImage&& image, const SE& se)
  {
    using I    = std::remove_reference_t<InputImage>;
    using Func = details::grad_op<image_value_t<I>>;
    using R    = std::invoke_result_t<Func, image_reference_t<I>, image_reference_t<I>>;
    using O    = image_ch_value_t<I, R>;

    auto opening = mln::morpho::opening(image, se);
    auto out     = mln::transform(opening, image, mln::morpho::details::grad_op<image_value_t<I>>());

    return static_cast<O&>(out);
  }

  template <class InputImage, class SE, class OutputImage>
  requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
  void top_hat(InputImage&& image, const SE& se, OutputImage&& out)
  {
    using I    = std::remove_reference_t<InputImage>;

    auto opening = mln::morpho::opening(image, se);
    mln::transform(opening, image, out, mln::morpho::details::grad_op<image_value_t<I>>());
  }

  namespace parallel
  {
    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> bottom_hat(InputImage&& image, const SE& se);

    template <class InputImage, class SE, class OutputImage>
    requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
    void bottom_hat(InputImage&& image, const SE& se, OutputImage&& out);

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> top_hat(InputImage&& image, const SE& se);

    template <class InputImage, class SE, class OutputImage>
    requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
    void top_hat(InputImage&& image, const SE& se, OutputImage&& out);


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> bottom_hat(InputImage&& image, const SE& se)
    {
      auto out = imconcretize(image).build();
      auto tmp = mln::morpho::MorphoPipeline(mln::morpho::e_MorphoPipelineOperation::Bot_hat, image, se, out);
      tmp.execute();
      return out;
    }

    template <class InputImage, class SE, class OutputImage>
    requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
    void bottom_hat(InputImage&& image, const SE& se, OutputImage&& out)
    {
      auto tmp = mln::morpho::MorphoPipeline(mln::morpho::e_MorphoPipelineOperation::Bot_hat, image, se, out);
      tmp.execute();
    }

    template <class InputImage, class SE>
    image_concrete_t<std::remove_reference_t<InputImage>> top_hat(InputImage&& image, const SE& se)
    {
      auto out = imconcretize(image).build();
      auto tmp = mln::morpho::MorphoPipeline(mln::morpho::e_MorphoPipelineOperation::Top_hat, image, se, out);
      tmp.execute();
      return out;
    }

    template <class InputImage, class SE, class OutputImage>
    requires mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value
    void top_hat(InputImage&& image, const SE& se, OutputImage&& out)
    {
      auto tmp = mln::morpho::MorphoPipeline(mln::morpho::e_MorphoPipelineOperation::Top_hat, image, se, out);
      tmp.execute();
    }
  } // namespace parallel

} // namespace mln::morpho
