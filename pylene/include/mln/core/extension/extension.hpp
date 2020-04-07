#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/neighborhood/neighborhood.hpp>


namespace mln
{

  namespace extension
  {

    /******************************************/
    /****          Free functions          ****/
    /******************************************/

    /// \brief Remove the extension of an image.
    ///
    /// \p remove_extension recursively removes the extensions of an image until
    /// getting an image without extension or an image whose extension cannot be
    /// removed. This function has to be overloaded by the morphers extending an
    /// image. This default implementation returns the input image as such.
    ///
    /// \param ima input image
    ///
    /// \return The image without extension
    ///
    template <typename I>
    const I& remove_extension(const Image<I>& ima);

    template <typename I>
    I& remove_extension(Image<I>& ima);

    template <typename I>
    I&& remove_extension(Image<I>&& ima);

    /// \brief Check if an image extension is wide enough to support
    /// a given neighborhood/se/window.
    ///
    /// An image does not need "adjutment" w.r.t to a neighborhood if
    /// these conditions are met:
    /// * \p ima has an extension
    /// * \p the neighborhood is constant (either static or dynamic)
    /// * \p the extension is wide enough
    template <class I, class N>
    bool need_adjust(const Image<I>& ima, const Neighborhood<N>& nbh);


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <typename I>
    const I& remove_extension(const Image<I>& ima)
    {
      return exact(ima);
    }

    template <typename I>
    I& remove_extension(Image<I>& ima)
    {
      return exact(ima);
    }

    template <typename I>
    I&& remove_extension(Image<I>&& ima)
    {
      return move_exact(ima);
    }

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

    template <class I, class N>
    bool need_adjust(const Image<I>& ima, const Neighborhood<N>& nbh)
    {
      return extension::impl::need_adjust(exact(ima), exact(nbh), image_extension_category_t<I>(),
                                          typename N::category());
    }

    template <class I, class SE>
    bool need_adjust(const mln::experimental::Image<I>& ima, const mln::experimental::StructuringElement<SE>& se)
    {
      return extension::impl::need_adjust(static_cast<const I&>(ima), static_cast<const SE&>(se),
                                          image_extension_category_t<I>(), typename SE::category());
    }


  } // namespace extension
} // end of namespace mln
