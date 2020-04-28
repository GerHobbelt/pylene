#pragma once

#include <mln/io/private/io.hpp>
#include <mln/io/private/freeimage_plugin.hpp>
#include <string>


namespace mln::io::experimental
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
    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(std::is_same<image_domain_t<I>, mln::experimental::box2d>() &&
                  "The domain must be a regular box2d");

    mln::io::internal::freeimage_writer_plugin p;
    mln::io::internal::save2d(std::forward<Image>(image), &p, filename);
  }

  template <class Image>
  void imsave(Image&& image, const std::string& filename)
  {
    imsave(std::forward<Image>(image), filename.c_str());
  }
}
