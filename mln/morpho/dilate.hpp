#ifndef MLN_MORPHO_DILATE_HPP
# define MLN_MORPHO_DILATE_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/extension/extension.hpp>
# include <mln/core/trace.hpp>
# include <mln/morpho/se/se.hpp>
# include <mln/kernel/kernel.hpp>
# include <mln/kernel/aggregate/max.hpp>

namespace mln
{

  namespace morpho
  {

    template <class I, class SE, class Compare = std::less<mln_value(I)> >
    mln_concrete(I)
    dilate(const Image<I>& ima,
           const StructuringElement<SE>& nbh,
           Compare cmp = Compare ());


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace impl
    {

      template <class I, class SE, class Compare, class J>
      void
      dilate_1(const I& ima, const SE& nbh, Compare cmp, J& out)
      {
        namespace ker = mln::kernel;
        using namespace mln::kernel::placeholders;

        auto expr = (g(p) = ker::aggregate::Max<Compare> (f(n)));
        ker::execute(expr, nbh, ima, out);
      }

      template <class I, class SE, class Compare, class J>
      void
      dilate_0(const I& ima, const SE& nbh, Compare cmp, J& out, std::true_type has_extension)
      {
        (void) has_extension;

        mln_value(I) v = value_traits<mln_value(I), Compare>::inf();

        if (extension::need_adjust(ima, nbh)) {
          mln::trace::warn("Slow version because input image extension is not wide enough.");
          dilate_1(extension::add_value_extension(ima, v), nbh, cmp,
                   out);
        } else {
          extension::fill(ima, v);
          dilate_1(ima, nbh, cmp, out);
        }
      }

      template <class I, class SE, class Compare, class J>
      void
      dilate_0(const I& ima, const SE& nbh, Compare cmp, J& out, std::false_type has_extension)
      {
        (void) has_extension;
        mln::trace::warn("Slow version because input image has no extension.");

        mln_value(I) v = value_traits<mln_value(I), Compare>::inf();
        dilate_1(extension::add_value_extension(ima, v), nbh, cmp, out);

      }


    }

    template <class I, class SE, class Compare>
    mln_concrete(I)
    dilate(const Image<I>& ima_, const StructuringElement<SE>& nbh_, Compare cmp)
    {
      mln::trace::entering("mln::morpho::dilate");
      const I& ima = exact(ima_);
      const SE& nbh = exact(nbh_);

      mln_concrete(I) out;
      resize(out, ima);

      mln::morpho::impl::dilate_0(ima, nbh, cmp, out, typename image_has_extension<I>::type ());

      mln::trace::exiting();
      return out;
    }


  } // end of namespace mln::morpho

} // end of namespace mln

#endif //!MLN_MORPHO_DILATE_HPP
