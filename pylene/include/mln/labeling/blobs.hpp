#pragma once


#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/core/trace.hpp>

#include <type_traits>
#include <vector>


namespace mln::labeling
{

  /// \brief labelize connected components of a binary image ima.
  template <typename Label, class I, class N>
  image_ch_value_t<I, Label> blobs(I ima, N nbh, int& nlabel);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {

    template <typename I, typename N, typename O>
    [[gnu::noinline]]
    int blobs_no_boundcheck(I& ima, N nbh, O& out)
    {
      using P = image_point_t<std::remove_reference_t<I>>;
      using Label = image_value_t<O>;
      mln_entering("mln::labeling::blobs (no_boundcheck)");

      constexpr int LBL_MAX = std::numeric_limits<Label>::max();

      int            nlabel = 0;
      std::vector<P> queue;
      queue.reserve(ima.domain().size());

      mln::extension::fill(out, LBL_MAX);

      mln_foreach(auto p, ima.domain())
      {
        if (!ima(p) || out(p) > 0)
          continue;


        queue.push_back(p);
        if (nlabel++ >= LBL_MAX)
          throw std::runtime_error("Detected overflow in the number of labels.");
        out(p) = nlabel;

        do
        {
          auto q = queue.back();
          queue.pop_back();
          for (auto n : nbh(q))
            if (out.at(n) == 0 && ima(n))
            {
              out.at(n) = nlabel;
              queue.push_back(n);
            }
        } while (not queue.empty());
      }
      return nlabel;
    }

    template <typename I, typename N, typename O>
    [[gnu::noinline]]
    int blobs_boundcheck(I& ima, N nbh, O& out)
    {
      using P               = image_point_t<std::remove_reference_t<I>>;
      using Label           = image_value_t<O>;
      constexpr int LBL_MAX = std::numeric_limits<Label>::max();

      mln_entering("mln::labeling::blobs (boundcheck)");

      int            nlabel = 0;
      std::vector<P> queue;
      queue.reserve(ima.domain().size());

      mln::extension::fill(out, LBL_MAX);

      auto dom = ima.domain();
      mln_foreach(auto p, dom)
      {
        if (!ima(p) || out(p) > 0)
          continue;


        queue.push_back(p);

        if (nlabel++ >= LBL_MAX)
          throw std::runtime_error("Detected overflow in the number of labels.");

        out(p) = nlabel;
        do
        {
          auto q = queue.back();
          queue.pop_back();

          for (auto n : nbh(q))
          {
            if (!dom.has(n)) // Bound check
              continue;

            if (out(n) == 0 && ima(n))
            {
              queue.push_back(n);
              out(n) = nlabel;
            }
          }
        } while (not queue.empty());
      }
      return nlabel;
    }
  } // namespace impl


  /// \brief labelize connected components of a binary image ima.
  template <typename Label, class I, class N>
  [[gnu::noinline]]
  image_ch_value_t<I, Label> blobs(I ima, N nbh, int& nlabel)
  {
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(std::is_convertible<image_value_t<I>, bool>::value, "Only supports binary image (type: bool)");

    image_build_error_code     status;
    image_ch_value_t<I, Label> out = imchvalue<Label>(ima)
                                         .adjust(nbh)       //
                                         .set_init_value(0) //
                                         .get_status(&status);
    if (status == IMAGE_BUILD_OK)
      nlabel = impl::blobs_no_boundcheck(ima, nbh, out);
    else
      nlabel = impl::blobs_boundcheck(ima, nbh, out);

    return out;
  }


} // namespace mln::labeling::
