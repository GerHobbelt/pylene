#pragma once

#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/value/indexer.hpp>
#include <mln/core/value/value_traits.hpp>

#include <algorithm>
#include <numeric>
#include <range/v3/range/concepts.hpp>
#include <range/v3/functional/concepts.hpp>
#include <vector>

namespace mln
{

  template <class InputImage, ::ranges::cpp20::range R, class Compare = std::less<image_value_t<InputImage>>>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  void sort_indexes(InputImage input, R&& rng, Compare cmp = Compare{});


  template <class InputImage, class Compare = std::less<image_value_t<InputImage>>>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  std::vector<image_index_t<InputImage>> sort_indexes(InputImage input, Compare cmp = Compare{});

  template <class InputImage, ::ranges::cpp20::range R, class Compare = std::less<image_value_t<InputImage>>>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  void sort_points(InputImage input, R&& rng, Compare cmp = Compare{});

  template <class InputImage, class Compare = std::less<image_value_t<InputImage>>>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  std::vector<image_point_t<InputImage>> sort_points(InputImage input, Compare cmp = Compare{});


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {
    template <class I, class Proj>
    void sort_compute_cumulated_histogram(I& input, int* histogram, std::size_t /* nvalues */, Proj proj)
    {
      // Comute histogram
      mln::for_each(input, [histogram, proj](image_value_t<I> v) { ++histogram[proj(v)]; });

      // Sum up values
      // This is what we should use after getting ride of indexer (and use value set / projection)
      // std::partial_sum(histogram, histogram + nvalues, histogram);

      using proj_value_t = typename Proj::index_type;

      int  count = 0;
      auto i     = value_traits<proj_value_t>::min();
      do
      {
        int tmp      = histogram[i];
        histogram[i] = count;
        count += tmp;
      } while (i++ < value_traits<proj_value_t>::max());
    }

    template <bool use_p, class I, class OutputRange, class Compare>
    void sort_by_counting_sort(I& input, OutputRange& rng, Compare /* cmp */)
    {
      mln_entering("mln::sort (counting sort)");

      using projecter_t  = indexer<image_value_t<I>, Compare>;
      using proj_value_t = typename projecter_t::index_type;

      constexpr std::size_t nvalues = 1 << value_traits<proj_value_t>::quant;

      projecter_t proj;
      int         histogram[nvalues] = {
          0,
      };
      sort_compute_cumulated_histogram(input, histogram, nvalues, proj);

      auto out = ::ranges::begin(rng);
      mln_foreach_new (auto px, input.pixels())
      {
        std::ptrdiff_t pos = histogram[proj(px.val())]++;
        if constexpr (use_p)
          out[pos] = px.point();
        else
          out[pos] = px.index();
      }
    }


    template <bool use_p, class I, class OutputRange, class Compare>
    void sort_by_quicksort(I& input, OutputRange& rng, Compare cmp)
    {
      mln_entering("mln::sort (quicksort)");

      // Copy to container
      {
        auto it = ::ranges::begin(rng);
        mln_foreach_new (auto px, input.pixels())
        {
          if constexpr (use_p)
            *it = px.point();
          else
            *it = px.index();
          ++it;
        }
      }

      // sort
      if constexpr (use_p) // By point
        std::sort(::ranges::begin(rng), ::ranges::end(rng),
                  [&input, cmp](const auto& x, const auto& y) { return cmp(input(x), input(y)); });
      else
        std::sort(::ranges::begin(rng), ::ranges::end(rng),
                  [&input, cmp](auto x, auto y) { return cmp(input[x], input[y]); });
    }


  } // namespace impl


  template <class InputImage, ::ranges::cpp20::range R, class Compare>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  void sort_indexes(InputImage input, R&& rng, Compare cmp)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>(), "Input is not an image.");
    static_assert(InputImage::indexable::value, "Input must be indexable.");
    static_assert(::ranges::cpp20::output_range<R, image_index_t<InputImage>>, "'rng' is not an output range");
    static_assert(::ranges::cpp20::random_access_range<R>, "'rng' must be random access (e.g. std::vector)");

    using V = image_value_t<InputImage>;

    // FIXME: use quantization for index type
    constexpr bool use_counting_sort = has_indexer<V, Compare>::value && (value_traits<V>::quant <= 18);

    if constexpr (use_counting_sort)
      impl::sort_by_counting_sort<false>(input, rng, cmp);
    else
      impl::sort_by_quicksort<false>(input, rng, cmp);
  }

  template <class InputImage, class Compare>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  std::vector<image_index_t<InputImage>> sort_indexes(InputImage input, Compare cmp)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>(), "Input is not an image.");
    static_assert(InputImage::indexable::value, "Input must be indexable.");

    std::vector<image_index_t<InputImage>> out(input.domain().size());
    mln::sort_indexes(std::move(input), out, cmp);
    return out;
  }

  template <class InputImage, ::ranges::cpp20::range R, class Compare>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  void sort_points(InputImage input, R&& rng, Compare cmp)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>(), "Input is not an image.");
    static_assert(InputImage::accessible::value, "Input must be accessible.");
    static_assert(::ranges::cpp20::output_range<R, image_point_t<InputImage>>, "'rng' is not an output range");
    static_assert(::ranges::cpp20::random_access_range<R>, "'rng' must be random access (e.g. std::vector)");

    using V = image_value_t<InputImage>;

    constexpr bool use_counting_sort = has_indexer<V, Compare>::value && (value_traits<V>::quant <= 18);

    if constexpr (use_counting_sort)
      impl::sort_by_counting_sort<true>(input, rng, cmp);
    else
      impl::sort_by_quicksort<true>(input, rng, cmp);
  }


  template <class InputImage, class Compare>
  requires ::ranges::cpp20::strict_weak_order<Compare, image_value_t<InputImage>, image_value_t<InputImage>>
  std::vector<image_point_t<InputImage>> sort_points(InputImage input, Compare cmp)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>(), "Input is not an image.");
    static_assert(InputImage::accessible::value, "Input must be accessible.");

    std::vector<image_point_t<InputImage>> out(input.domain().size());
    mln::sort_points(std::move(input), out, cmp);
    return out;
  }


} // namespace mln::
