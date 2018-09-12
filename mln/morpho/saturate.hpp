#ifndef MLN_CORE_MORPHO_SATURATE_HPP
#define MLN_CORE_MORPHO_SATURATE_HPP

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>
#include <queue>

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
        J& out = exact(out_);

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
    }

    template <typename I, typename N, typename J>
    void saturate(const Image<I>& ima, const N& nbh, const mln_point(I) & pinf, Image<J>& out)
    {
      static_assert(std::is_convertible<mln_value(I), bool>::value,
                    "Input image value type must be convertible to bool");

      static_assert(std::is_convertible<bool, mln_value(J)>::value, "Output image value type must be bool");

      mln::fill(out, true);

      if (!mln::extension::need_adjust(out, nbh))
      {
        extension::fill(out, false);
        impl::saturate(ima, nbh, out, pinf);
      }
      else
      {
        auto tmp = mln::extension::add_value_extension(out, false);
        impl::saturate(ima, nbh, tmp, pinf);
      }
    }

    template <typename I, typename N>
    mln_ch_value(I, bool) saturate(const Image<I>& ima_, const N& nbh, const mln_point(I) & pinf)
    {
      static_assert(std::is_convertible<mln_value(I), bool>::value,
                    "Input image value type must be convertible to bool");

      const I& ima = exact(ima_);

      mln_ch_value(I, bool) out = imchvalue<bool>(ima).adjust(nbh).init(true);
      mln::morpho::saturate(ima, nbh, pinf, out);
      return out;
    }
  }
}

#endif // ! MLN_CORE_MORPHO_SATURATE_HPP
