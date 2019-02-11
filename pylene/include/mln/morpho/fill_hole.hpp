#pragma once

#include <mln/core/image/image.hpp>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/extension/fill.hpp>

#include <queue>

namespace mln::morpho
{


  /// \brief Fill the objects holes. The background connected components than are not connected to the marker point \p
  /// marker are removed from the background.
  ///
  /// \param input Input image
  /// \param out Output image
  /// \param marker marker point
  template <typename I, typename N>
  image_ch_value_t<I, bool> //
  fill_hole(I input, N nbh, image_point_t<I> marker);

  template <typename I, typename N, typename J>
  void fill_hole(I input, N nbh, image_point_t<I> marker, J out);


  /*******************/
  /** Implementation */
  /*******************/

  namespace impl
  {

    template <typename I, typename N, typename J>
    [[gnu::noinline]]
    void fill_hole(I&& input, N nbh, J&& out, image_point_t<std::remove_reference_t<I>> pinf)
    {
      using P = image_point_t<std::remove_reference_t<I>>;

      std::queue<P> queue;

      if (!input(pinf))
        queue.push(pinf);

      while (!queue.empty())
      {
        auto p = queue.front();
        queue.pop();
        for (auto n : nbh(p))
          if (out.at(n) and not input(n))
          {
            out(n) = false;
            queue.push(n);
          }
      }
    }
  } // namespace impl


  template <typename I, typename N, typename J>
  void fill_hole(I input, N nbh, image_point_t<I> marker, J out)
  {
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<J, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());

    static_assert(std::is_convertible<image_value_t<I>, bool>::value, "Input image value type must be convertible to bool");
    static_assert(std::is_same<image_value_t<J>, bool>::value, "Output image value type must be bool");

    if (!mln::extension::fit(out, nbh))
      throw std::runtime_error("The output image extension is not wide enough.");

    mln::fill(out, true);
    mln::extension::fill(out, false);
    impl::fill_hole(input, nbh, out, marker);
  }

  template <typename I, typename N>
  image_ch_value_t<I, bool> //
  fill_hole(I input, N nbh, image_point_t<I> marker)

  {
    static_assert(std::is_convertible_v<image_value_t<I>, bool>, "Input image value type must be convertible to bool");

    image_build_error_code status;
    image_ch_value_t<I, bool> out = imchvalue<bool>(input)
                                        .adjust(nbh)          //
                                        .set_init_value(true) //
                                        .get_status(&status);

    if (status != IMAGE_BUILD_OK)
      throw std::runtime_error("The output image extension is not wide enough.");

    mln::extension::fill(out, false);
    impl::fill_hole(input, nbh, out, marker);
    return out;
  }

} // namespace mln::morpho::
