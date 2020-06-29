#pragma once


#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/private/ndbuffer_image.hpp>


namespace mln
{
  template <class T>
  using image1d = mln::__ndbuffer_image<T, 1>;

  template <class T>
  using image2d = mln::__ndbuffer_image<T, 2>;

  template <class T>
  using image3d = mln::__ndbuffer_image<T, 3>;
}
