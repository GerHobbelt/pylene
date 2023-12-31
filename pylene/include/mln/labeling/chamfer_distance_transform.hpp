#pragma once


#include <mln/core/concepts/neighborhood.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/neighborhood/neighborhood.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/core/range/view/reverse.hpp>
#include <mln/core/range/view/zip.hpp>
#include <mln/core/trace.hpp>

#include <limits>


namespace mln::labeling
{

  template <typename DistanceType = int, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, DistanceType> //
  chamfer_distance_transform(InputImage&& f, Neighborhood&& nbh, bool background_is_object = false);

  /**********************************/
  /**   Implementation            ***/
  /**********************************/

  namespace internal
  {

    template <class T>
    struct add_saturate
    {
      using promoted_t = decltype(std::declval<T>() + std::declval<T>());

      auto operator()(T x, T y) const -> promoted_t
      {
        if (x > (std::numeric_limits<T>::max() - y))
          return std::numeric_limits<T>::max();
        else
          return static_cast<T>(x + y);
      }
    };
  } // namespace internal

  namespace impl
  {
    template <class I, class N, class O, class SumOp>
    void chamfer_distance_transform(I input, N nbh, O output, SumOp add)
    {
      static_assert(mln::is_a<I, mln::details::Image>());
      static_assert(mln::is_a<O, mln::details::Image>());
      static_assert(mln::is_a<N, mln::details::Neighborhood>());

      static_assert(std::is_convertible<image_value_t<I>, bool>::value, "Input value type must be convertible to bool");

      mln_entering("mln::transform::chamfer_distance_transform");

      using distance_t = image_value_t<O>;
      using promoted_t = decltype(std::declval<distance_t>() + std::declval<distance_t>());

      // Forward scan
      {
        auto zz = mln::ranges::view::zip(input.pixels(), output.pixels());
        mln_foreach((auto [pxin, pxout]), zz)
        {
          if (!pxin.val())
          {
            pxout.val() = 0;
            continue;
          }

          promoted_t vmin = std::numeric_limits<distance_t>::max();
          for (auto qx : nbh.before(pxout))
            vmin = std::min(vmin, static_cast<promoted_t>(add(qx.val(), qx.weight())));
          pxout.val() = static_cast<distance_t>(vmin);
        }
      }

      // Backward
      {
        auto zz = mln::ranges::view::zip(input.pixels(), output.pixels());
        mln_foreach((auto [pxin, pxout]), mln::ranges::view::reverse(zz))
        {
          if (!pxin.val())
            continue;

          promoted_t vmin = pxout.val();
          for (auto qx : nbh.after(pxout))
            vmin = std::min(vmin, static_cast<promoted_t>(add(qx.val(), qx.weight())));
          pxout.val() = static_cast<distance_t>(vmin);
        }
      }
    }
  } // namespace impl

  template <typename DistanceType, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, DistanceType> //
  chamfer_distance_transform(InputImage&& f, Neighborhood&& nbh, bool background_is_object)
  {
    using I = std::remove_reference_t<InputImage>;
    using N = std::remove_reference_t<Neighborhood>;

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());

    image_build_error_code err = IMAGE_BUILD_OK;

    auto out = imchvalue<DistanceType>(f) //
                   .adjust(nbh)
                   .get_status(&err)
                   .build();

    DistanceType vfill = (background_is_object) ? std::numeric_limits<DistanceType>::max() : 0;


    if (err == IMAGE_BUILD_OK)
    {
      mln::extension::try_fill(out, vfill);
      impl::chamfer_distance_transform(f, nbh, out, internal::add_saturate<DistanceType>{});
    }
    else
    {
      auto output = mln::view::value_extended(out, vfill);
      impl::chamfer_distance_transform(f, nbh, output, internal::add_saturate<DistanceType>{});
    }

    return out;
  }

} // namespace mln::labeling::
