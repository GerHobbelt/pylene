#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/private/traits/extension.hpp>


namespace mln
{

  namespace extension
  {

    /******************************************/
    /****          Free functions          ****/
    /******************************************/


    /// \brief Check if an image extension is wide enough to support
    /// a given neighborhood/se/window.
    ///
    /// An image does not need "adjutment" w.r.t to a neighborhood if
    /// these conditions are met:
    /// * \p ima has an extension
    /// * \p the neighborhood is constant (either static or dynamic)
    /// * \p the extension is wide enough
    template <class I, class SE>
    bool need_adjust(const mln::details::Image<I>& ima, const mln::details::StructuringElement<SE>& se);


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace impl
    {

      template <class I, class N>
      bool need_adjust(const I&, const N&, extension_tag, adaptative_neighborhood_tag)
      {
        return true;
      }

      template <class I, class N>
      bool need_adjust(const I&, const N&, value_extension_tag, adaptative_neighborhood_tag)
      {
        return false;
      }

      template <class I, class N>
      bool need_adjust(const I& ima, const N& nbh, border_extension_tag, dynamic_neighborhood_tag)
      {
        return nbh.radial_extent() > ima.border();
      }
    } // namespace impl


    template <class I, class SE>
    bool need_adjust(const mln::details::Image<I>& ima, const mln::details::StructuringElement<SE>& se)
    {
      return extension::impl::need_adjust(static_cast<const I&>(ima), static_cast<const SE&>(se),
                                          image_extension_category_t<I>(), typename SE::category());
    }


  } // namespace extension
} // end of namespace mln
