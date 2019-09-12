#pragma once


#include <mln/core/algorithm/copy.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>


/// \file
/// \ingroup morpho

namespace mln::morpho::experimental
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
  image_concrete_t<std::remove_reference_t<I>>
  hit_or_miss(I&& input, const SEh& se_hit, const SEm& se_miss);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {

    template <class I, class SEh, class SEm, class OutputImage>
    void hit_or_miss(I& f, const SEh& seh, const SEm& sem, OutputImage& out)
    {
      using namespace mln::view::ops;
      using V = image_value_t<I>;
      mln_entering("mln::morpho::hit_or_miss");


      auto ero = morpho::experimental::erosion(f, seh);
      auto dil = morpho::experimental::dilation(f, sem);

      if constexpr (std::is_same_v<V, bool>)
      {
        auto res = ero and (not dil);
        mln::experimental::copy(res, out);
      }
      else
      {
        V    zero = {};
        auto res  = mln::view::ifelse(dil < ero, ero - dil, zero);
        mln::experimental::copy(res, out);
      }
    }
  }


  template <class I, class SEh, class SEm, class OutputImage>
  void hit_or_miss(I& f, const SEh& se_hit, const SEm& se_miss, OutputImage& out)
  {
    static_assert(mln::is_a<std::remove_reference_t<I>, mln::experimental::Image>());
    static_assert(mln::is_a<std::remove_reference_t<OutputImage>, mln::experimental::Image>());
    static_assert(mln::is_a<SEh, mln::experimental::StructuringElement>());
    static_assert(mln::is_a<SEm, mln::experimental::StructuringElement>());

    impl::hit_or_miss(f, se_hit, se_miss, out);
  }



  template <class I, class SEh, class SEm>
  image_concrete_t<std::remove_reference_t<I>> hit_or_miss(I&& f, const SEh& se_hit, const SEm& se_miss)
  {
    static_assert(mln::is_a<std::remove_reference_t<I>, mln::experimental::Image>());
    static_assert(mln::is_a<SEh, mln::experimental::StructuringElement>());
    static_assert(mln::is_a<SEm, mln::experimental::StructuringElement>());

    auto out = imconcretize(f).build();
    impl::hit_or_miss(f, se_hit, se_miss, out);
    return out;
  }

} // namespace mln::morpho::experimental
