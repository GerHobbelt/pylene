#pragma once
#include <ano/core/image/image.hpp>
#include <ano/core/range/rows.hpp>


namespace ano
{
  template <class OutputImage, class Generator>
  void generate(OutputImage output, Generator g);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class OutputImage, class Generator>
  void generate(OutputImage output, Generator g)
  {
    static_assert(ano::is_a<OutputImage, ano::details::Image>());
    static_assert(std::is_convertible_v<std::invoke_result_t<Generator>, image_value_t<OutputImage>>);

    auto&& vals = output.values();
    for (auto row : ano::ranges::rows(vals))
      for (auto& v : row)
        v = g();
  }

} // namespace ano
