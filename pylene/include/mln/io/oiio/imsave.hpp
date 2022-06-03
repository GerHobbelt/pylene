#pragma once

#include <mln/io/private/io.hpp>
#include <mln/io/private/openimageio_plugin.hpp>
#include <string>


namespace mln::io::oiio
{
  template <class I>
  void imsave(I&& image, const std::string& filename);

  template <class I>
  void imsave(I&& image, const char* filename);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class Image>
  void imsave(Image&& image, const char* filename)
  {
    using I = std::remove_reference_t<Image>;
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(std::is_same<image_domain_t<I>, mln::box2d>() && "The domain must be a regular box2d");

    mln::io::oiio::internal::openimageio_writer_plugin p;
    mln::io::internal::save2d(std::forward<Image>(image), &p, filename);
  }

  template <class Image>
  void imsave(Image&& image, const std::string& filename)
  {
    imsave(std::forward<Image>(image), filename.c_str());
  }
} // namespace mln::io::oiio
