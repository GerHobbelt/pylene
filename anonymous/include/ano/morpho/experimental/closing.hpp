#pragma once


#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/extension/border_management.hpp>
#include <ano/core/trace.hpp>
#include <ano/morpho/dilation.hpp>
#include <ano/morpho/erosion.hpp>

namespace ano::morpho::experimental
{
  /// \brief Compute the structural morphological closing.
  ///
  /// The structurual morphological closing γ of \p f by a structuring
  /// element 𝓑
  /// \f[
  /// \gamma(f) = \varepsilon_\mathcal{B}(\delta_\mathcal{B}(f))
  /// \f]
  /// where ε denotes the erosion and δ the dilation.
  ///
  ///
  /// + If the optional \p out image is provided, it must be wide enough to store
  ///   the results (the function does not perform any resizing).
  ///
  /// + A border manager can be supplied for side-effects
  ///
  /// \param[in] input Input image \p f
  /// \param[in] se Structuring element/Neighborhood/Window \p B to look around.
  /// \param[out] out (optional) Output image \p g to write in.
  /// \param[in] bm (optional) Border management function
  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void closing(InputImage&& image, const ano::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out);

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> closing(InputImage&&                       image,
                                                                const ano::StructuringElement<SE>& se);

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  closing(InputImage&& image, const ano::StructuringElement<SE>& se, BorderManager bm,
          std::enable_if_t<!ano::is_a<BorderManager, ano::details::Image>::value>* = nullptr);

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<ano::is_a<std::remove_reference_t<OutputImage>, ano::details::Image>::value>
  closing(InputImage&& image, const ano::StructuringElement<SE>& se, OutputImage&& out);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void closing(InputImage&& image, const ano::StructuringElement<SE>& se, BorderManager bm, OutputImage&& out)
  {
    ano_entering("ano::morpho::experimental::closing");

    // To enable when we can concept check that domain are comparable
    // assert(image.domain() == out.domain());

    if (!(bm.method() == ano::extension::BorderManagementMethod::Fill ||
          bm.method() == ano::extension::BorderManagementMethod::User))
      throw std::runtime_error("Invalid border management method (should be FILL or USER)");

    auto tmp = ano::morpho::experimental::dilation(image, se, bm);
    ano::morpho::experimental::erosion(tmp, se, bm, out);
  }

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<ano::is_a<std::remove_reference_t<OutputImage>, ano::details::Image>::value>
  closing(InputImage&& image, const ano::StructuringElement<SE>& se, OutputImage&& out)
  {
    ano_entering("ano::morpho::experimental::closing");
    auto tmp = ano::morpho::experimental::dilation(image, se);
    ano::morpho::experimental::erosion(tmp, se, out);
  }

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> closing(InputImage&&                       image,
                                                                const ano::StructuringElement<SE>& se)
  {
    auto out = imconcretize(image).build();
    ano::morpho::experimental::closing(image, se, out);
    return out;
  }

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  closing(InputImage&& image, const ano::StructuringElement<SE>& se, BorderManager bm,
          std::enable_if_t<!ano::is_a<BorderManager, ano::details::Image>::value>*)
  {
    auto out = imconcretize(image).build();
    ano::morpho::experimental::closing(image, se, bm, out);
    return out;
  }


} // namespace ano::morpho::experimental
