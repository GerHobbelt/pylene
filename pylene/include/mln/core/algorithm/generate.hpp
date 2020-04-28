#pragma once
#include <mln/core/image/image.hpp>
#include <mln/core/range/rows.hpp>


namespace mln
{
  template <class OutputImage, class Generator>
  void generate(OutputImage output, Generator g);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class OutputImage, class Generator>
  void generate(OutputImage output, Generator g)
  {
    static_assert(mln::is_a<OutputImage, experimental::Image>());
    static_assert(std::is_convertible_v<std::invoke_result_t<Generator>, image_value_t<OutputImage>>);

    auto&& vals = output.new_values();
    for (auto row : mln::ranges::rows(vals))
      for (auto& v : row)
        v = g();
  }

} // namespace mln
