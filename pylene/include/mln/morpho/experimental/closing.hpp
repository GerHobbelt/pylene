#pragma once


#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/trace.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>

namespace mln::morpho::experimental
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
  void closing(InputImage&& image, const mln::StructuringElement<SE>& se, BorderManager bm,
               OutputImage&& out);

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> closing(InputImage&&                                     image,
                                                                const mln::StructuringElement<SE>& se);

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  closing(InputImage&& image, const mln::StructuringElement<SE>& se, BorderManager bm,
           std::enable_if_t<!mln::is_a<BorderManager, mln::details::Image>::value>* = nullptr);

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value>
  closing(InputImage&& image, const mln::StructuringElement<SE>& se, OutputImage&& out);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void closing(InputImage&& image, const mln::StructuringElement<SE>& se, BorderManager bm,
                OutputImage&& out)
  {
    mln_entering("mln::morpho::experimental::closing");

    // To enable when we can concept check that domain are comparable
    // assert(image.domain() == out.domain());

    if (! (bm.method() == mln::extension::BorderManagementMethod::Fill || bm.method() == mln::extension::BorderManagementMethod::User))
      throw std::runtime_error("Invalid border management method (should be FILL or USER)");

    auto tmp = mln::morpho::experimental::dilation(image, se, bm);
    mln::morpho::experimental::erosion(tmp, se, bm, out);
  }

  template <class InputImage, class SE, class OutputImage>
  std::enable_if_t<mln::is_a<std::remove_reference_t<OutputImage>, mln::details::Image>::value>
  closing(InputImage&& image, const mln::StructuringElement<SE>& se, OutputImage&& out)
  {
    mln_entering("mln::morpho::experimental::closing");
    auto tmp = mln::morpho::experimental::dilation(image, se);
    mln::morpho::experimental::erosion(tmp, se, out);
  }

  template <class InputImage, class SE>
  image_concrete_t<std::remove_reference_t<InputImage>> closing(InputImage&& image, const mln::StructuringElement<SE>& se)
  {
    auto out = imconcretize(image).build();
    mln::morpho::experimental::closing(image, se, out);
    return out;
  }

  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>> closing(InputImage&& image, const mln::StructuringElement<SE>& se, BorderManager bm,
                                                                 std::enable_if_t<!mln::is_a<BorderManager, mln::details::Image>::value>*)
  {
    auto out = imconcretize(image).build();
    mln::morpho::experimental::closing(image, se, bm, out);
    return out;
  }


} // namespace mln::morpho::experimental