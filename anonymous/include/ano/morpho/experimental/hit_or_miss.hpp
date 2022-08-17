#pragma once


#include <ano/core/algorithm/copy.hpp>
#include <ano/core/image/view/operators.hpp>
#include <ano/morpho/experimental/dilation.hpp>
#include <ano/morpho/experimental/erosion.hpp>


/// \file
/// \ingroup morpho

namespace ano::morpho::experimental
{

  /// \ingroup morpho
  /// \brief (Unconstrained) Hit-or-miss transform
  ///
  /// The hit-or-miss transform is non-linerar filter used to detect
  /// pattern in images. It is defined as:
  ///
  /// \f[
  /// UHTM_\mathcal{B}(f)(x) =
  /// \begin{cases}
  /// \varepsilon_{B_{fg}}(x) - \delta_{B_{bg}}(x) & \mathrm{if} \varepsilon_{B_{fg}}(x) > \delta_{B_{bg}}(x)\newline
  /// 0 & otherwise
  /// \end{cases}
  /// \f]
  /// where \f$ \mathcal{B} = (B_{fg}, B_{bg}) \f$ is the composite structuring element
  /// composed by the two disjoint foreground and background SEs.
  ///
  /// \param ima Input image 𝑓
  /// \param se_hit  Structuring element for foreground
  /// \param se_miss  Structuring element for background
  /// \param out (optional) Output image 𝑔 so store the result.
  template <class InputImage, class SEh, class SEm, class OutputImage>
  void hit_or_miss(InputImage&& input, const SEh& se_hit, const SEm& se_miss, OutputImage&& out);

  template <class I, class SEh, class SEm>
  image_concrete_t<std::remove_reference_t<I>> hit_or_miss(I&& input, const SEh& se_hit, const SEm& se_miss);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {

    template <class I, class SEh, class SEm, class OutputImage>
    void hit_or_miss(I& f, const SEh& seh, const SEm& sem, OutputImage& out)
    {
      using namespace ano::view::ops;
      using V = image_value_t<I>;
      ano_entering("ano::morpho::hit_or_miss");


      auto ero = morpho::experimental::erosion(f, seh);
      auto dil = morpho::experimental::dilation(f, sem);

      if constexpr (std::is_same_v<V, bool>)
      {
        auto res = ero and (not dil);
        ano::copy(res, out);
      }
      else
      {
        V    zero = {};
        auto res  = ano::view::ifelse(dil < ero, ero - dil, zero);
        ano::copy(res, out);
      }
    }
  } // namespace impl


  template <class I, class SEh, class SEm, class OutputImage>
  void hit_or_miss(I& f, const SEh& se_hit, const SEm& se_miss, OutputImage& out)
  {
    static_assert(ano::is_a<std::remove_reference_t<I>, ano::details::Image>());
    static_assert(ano::is_a<std::remove_reference_t<OutputImage>, ano::details::Image>());
    static_assert(ano::is_a<SEh, ano::details::StructuringElement>());
    static_assert(ano::is_a<SEm, ano::details::StructuringElement>());

    impl::hit_or_miss(f, se_hit, se_miss, out);
  }


  template <class I, class SEh, class SEm>
  image_concrete_t<std::remove_reference_t<I>> hit_or_miss(I&& f, const SEh& se_hit, const SEm& se_miss)
  {
    static_assert(ano::is_a<std::remove_reference_t<I>, ano::details::Image>());
    static_assert(ano::is_a<SEh, ano::details::StructuringElement>());
    static_assert(ano::is_a<SEm, ano::details::StructuringElement>());

    auto out = imconcretize(f).build();
    impl::hit_or_miss(f, se_hit, se_miss, out);
    return out;
  }

} // namespace ano::morpho::experimental
