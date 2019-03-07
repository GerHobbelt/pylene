#pragma once

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>

#include <queue>


// FIXME:
namespace to_migrate
{
  namespace impl
  {
    template <typename I, typename V>
    void fill(I& ima, const V& v)
    {
      mln_viter(pin, ima);
      mln_forall (pin)
        *pin = v;
    }
  } // namespace impl

  template <typename OutputImage, typename Value>
  [[deprecated]] OutputImage& __fill(mln::Image<OutputImage>& output_, const Value& val) {
    OutputImage& output = mln::exact(output_);
    impl::fill(output, val);
    return output;
  }

  template <typename OutputImage, typename Value>
  [[deprecated]] OutputImage&& __fill(mln::Image<OutputImage>&& output_, const Value& val)
  {
    __fill(output_, val);
    return mln::move_exact(output_);
  }
} // namespace to_migrate

namespace mln
{

  namespace morpho
  {

    template <typename I, typename N, typename J>
    void saturate(const Image<I>& ima, const N& nbh, const mln_point(I) & pinf, Image<J>& out);

    template <typename I, typename N>
    mln_ch_value(I, bool) saturate(const Image<I>& ima, const N& nbh, const mln_point(I) & pinf);

    /*******************/
    /** Implementation */
    /*******************/

    namespace impl
    {

      template <typename I, typename N, typename J>
      void saturate(const Image<I>& ima_, const N& nbh, Image<J>& out_, const mln_point(I) & pinf)
      {
        const I& ima = exact(ima_);
        J&       out = exact(out_);

        std::queue<mln_point(I)> queue;

        if (!ima(pinf))
          queue.push(pinf);

        mln_point(I) p;
        mln_iter(n, nbh(p));
        while (!queue.empty())
        {
          p = queue.front();
          queue.pop();
          mln_forall (n)
            if (out.at(*n) and not ima(*n))
            {
              out(*n) = false;
              queue.push(*n);
            }
        }
      }
    } // namespace impl

    template <typename I, typename N, typename J>
    void saturate(const Image<I>& ima, const N& nbh, const mln_point(I) & pinf, Image<J>& out)
    {
      static_assert(std::is_convertible<mln_value(I), bool>::value,
                    "Input image value type must be convertible to bool");

      static_assert(std::is_same<mln_value(J), bool>::value, "Output image value type must be bool");

      // FIXME:
      ::to_migrate::__fill(out, true);
      extension::fill(out, false);
      impl::saturate(ima, nbh, out, pinf);
    }

    template <typename I, typename N>
    mln_ch_value(I, bool) saturate(const Image<I>& ima_, const N& nbh, const mln_point(I) & pinf)
    {
      static_assert(std::is_convertible<mln_value(I), bool>::value,
                    "Input image value type must be convertible to bool");

      const I& ima = exact(ima_);

      int status;
      mln_ch_value(I, bool) out = imchvalue<bool>(ima).adjust(nbh).init(true).get_status(status);

      if (status == 0)
      {
        extension::fill(out, false);
        impl::saturate(ima, nbh, out, pinf);
      }
      else
      {
        std::abort();
      }
      return out;
    }
  } // namespace morpho
} // namespace mln
