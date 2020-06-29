#pragma once

#include <mln/core/algorithm/for_each.hpp>

#include <vector>

#include <concepts/concepts.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/view/span.hpp>


namespace mln::data
{

  template <class I, class OutputRng>
  [[gnu::noinline]]
  void histogram(I image, OutputRng histogram);


  template <class I>
  [[gnu::noinline]]
  std::vector<std::size_t> histogram(I image);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace impl
  {
    template <class I, class OutputRng>
    void histogram(I&& image, OutputRng hist)
    {
      using V = image_value_t<std::remove_reference_t<I>>;

      static_assert(::concepts::unsigned_integral<V>);

      ::ranges::fill(hist, 0);
      mln::for_each(std::forward<I>(image), [&hist](int x) { hist[x]++; });
    }
  }

  template <class I, class OutputRng>
  [[gnu::noinline]]
  void histogram(I image, OutputRng histogram)
  {
    impl::histogram(image, histogram);
  }

  template <class I>
  [[gnu::noinline]]
  std::vector<std::size_t> histogram(I image)
  {
    using V = image_value_t<I>;
    static_assert(::concepts::unsigned_integral<V>);

    std::vector<std::size_t> histogram;
    histogram.resize(std::numeric_limits<V>::max() + 1);

    impl::histogram(image, ::ranges::make_span(histogram.data(), histogram.size()));
    return histogram;
  }

}
