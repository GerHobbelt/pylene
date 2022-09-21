#pragma once


#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/accu/accumulators/max.hpp>

#include <concepts>

namespace mln::filters
{

  /*
   * \brief Compute the peaks after non-maximum suppression.
   * \param min_xdistance The x-size of the NMS window
   * \param min_ydistance The y-size of the NMS window
   * \param min_intensity The minimum intensity of the peak (if null, 0.5 * the max peak used)
   */
  template <std::totally_ordered T>
  std::vector<point2d> nms(image2d<T>                             input,
                           int                                    min_xdistance,                //
                           int                                    min_ydistance,                //
                           std::optional<std::type_identity_t<T>> min_intensity = std::nullopt, //
                           int                                    num_peaks     = -1,           //
                           image2d<T>*                            nms_image     = nullptr);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <std::totally_ordered T>
  std::vector<point2d> nms(image2d<T> input, int min_xdistance, int min_ydistance,
                           std::optional<std::type_identity_t<T>> min_intensity, int num_peaks, image2d<T>* nms_image)
  {
    auto r   = mln::se::rect2d{min_xdistance, min_ydistance};
    auto dil = mln::morpho::dilation(input, r);

    if (num_peaks <= 0)
      num_peaks = INT_MAX;

    T minv;
    if (min_intensity.has_value())
    {
      minv = min_intensity.value();
    }
    else
    {
      minv = 0.5f * mln::accumulate(input, accu::accumulators::max<T>());
    }

    auto cmp = [&input](point2d p, point2d q) { return input(p) > input(q); };

    std::vector<point2d> vec;
    if (num_peaks != INT_MAX)
      vec.reserve(num_peaks);

    {
      auto zz = mln::ranges::view::zip(input.pixels(), dil.pixels());
      mln_foreach((auto [px_v, px_m]), zz)
      {
        if (px_v.val() == px_m.val() && px_v.val() >= minv)
        {
          if (vec.size() < static_cast<std::size_t>(num_peaks))
          {
            vec.push_back(px_v.point());
            std::push_heap(vec.begin(), vec.end(), cmp);
          }
          else if (px_v.val() > input(vec[0]))
          {
            std::pop_heap(vec.begin(), vec.end(), cmp);
            vec.back() = px_v.point();
            std::push_heap(vec.begin(), vec.end(), cmp);
          }

        }
      }
    }

    if (nms_image)
      *nms_image = std::move(dil);

    std::sort_heap(vec.begin(), vec.end(), cmp);
    return vec;
  }

}
