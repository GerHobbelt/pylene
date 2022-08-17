#pragma once


#include <ano/core/canvas/local_accumulation.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/extension/border_management.hpp>
#include <ano/core/trace.hpp>

#include <ano/accu/accumulators/h_rank.hpp>

namespace ano::morpho
{

  /// \brief Rank filter
  ///
  /// A rank filter is non-linerar filter using the value ordering to
  /// compute the filtered value. It computes:
  /// \f[
  /// g(x) = r(\{ f(y) \in \mathcal{B}_x) \})
  /// \f]
  /// where \p r returns the 𝑟-th value of the set of pixels of the
  /// structuring element 𝑩 centered in 𝑥.
  ///
  ///
  /// \param ima Input image 𝑓
  /// \param se  Structuring element
  /// \param out (optional) Output image 𝑔 so store the result.
  /// \tparam Ratio A std::ratio type that defines the rank of the filtered value.
  template <class Ratio, class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  rank_filter(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm);

  template <class Ratio, class InputImage, class SE, class BorderManager, class OutputImage>
  void rank_filter(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm,
                   OutputImage&& out);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class Ratio, class InputImage, class SE, class BorderManager, class OutputImage>
  void rank_filter(InputImage&& input, const ano::details::StructuringElement<SE>& se, BorderManager bm,
                   OutputImage&& out)
  {
    using I = std::remove_reference_t<InputImage>;
    static_assert(ano::is_a<I, ano::details::Image>::value);

    using V = image_value_t<I>;
    ano_entering("ano::morpho::rank_filter");

    // To enable when we can concept check that domain are comparable
    // assert(image.domain() == out.domain());

    auto [f, ses] = bm.manage(input, static_cast<const SE&>(se));

    std::visit(
        [&out](auto&& f, auto&& se) {
          ano::accu::accumulators::h_rank<V, Ratio> accu;
          ano::canvas::LocalAccumulation            algo(accu, se, f, out);
          algo.Execute();
        },
        f, ses);
  }


  template <class Ratio, class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  rank_filter(InputImage&& image, const ano::details::StructuringElement<SE>& se, BorderManager bm)
  {
    using I = std::remove_reference_t<InputImage>;
    static_assert(ano::is_a<I, ano::details::Image>::value);

    image_concrete_t<I> out = imconcretize(image);
    rank_filter<Ratio>(image, se, bm, out);
    return out;
  }

} // namespace ano::morpho
