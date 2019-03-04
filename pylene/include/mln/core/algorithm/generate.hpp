#pragma once
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>


namespace mln
{
  namespace experimental
  {
    template <class OutputImage, class Generator>
    void generate(OutputImage output, Generator g);
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace experimental
  {
    template <class OutputImage, class Generator>
    void generate(OutputImage output, Generator g)
    {
      static_assert(mln::is_a<OutputImage, Image>());
      static_assert(std::is_convertible_v<std::invoke_result_t<Generator>, image_value_t<OutputImage>>);

      auto&& vals = output.new_values();
      for (auto row : mln::ranges::rows(vals))
        for (auto& v : row)
          v = g();
    }
  } // namespace experimental

} // namespace mln


