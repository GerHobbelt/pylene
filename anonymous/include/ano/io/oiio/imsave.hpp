#pragma once

#include <ano/io/private/io.hpp>
#include <ano/io/private/openimageio_plugin.hpp>
#include <string>


namespace ano::io::oiio
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
    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(std::is_same<image_domain_t<I>, ano::box2d>() && "The domain must be a regular box2d");

    ano::io::oiio::internal::openimageio_writer_plugin p;
    ano::io::internal::save2d(std::forward<Image>(image), &p, filename);
  }

  template <class Image>
  void imsave(Image&& image, const std::string& filename)
  {
    imsave(std::forward<Image>(image), filename.c_str());
  }
} // namespace ano::io::oiio
