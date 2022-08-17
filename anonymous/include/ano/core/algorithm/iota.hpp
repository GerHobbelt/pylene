#pragma once
#include <ano/core/image/image.hpp>
#include <ano/core/range/rows.hpp>

namespace ano
{

  /// \brief \p iota traverses the image, the `i`th pixel is assigned
  /// with `val + i`
  ///
  /// \ingroup Algorithms
  ///
  /// \param[out] ima The output image.
  /// \param val The value to assign.
  ///
  /// \return The image.
  ///
  /// \tparam I is a model of the Writable Forward Image.
  /// \tparam Value must be convertible to Image's value type.
  template <typename OutputImage, typename Value>
  void iota(OutputImage output, Value val);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename OutputImage, typename Value>
  void iota(OutputImage output, Value val)
  {
    static_assert(ano::is_a<OutputImage, ano::details::Image>::value);
    static_assert(std::is_convertible_v<Value, image_value_t<OutputImage>>);

    auto&& vals = output.values();
    for (auto row : ano::ranges::rows(vals))
      for (auto& v : row)
        v = val++;
  }

} // namespace ano
