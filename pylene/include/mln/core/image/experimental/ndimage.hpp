#pragma once


#include <mln/core/image/experimental/ndbuffer_image.hpp>
#include <mln/core/image/experimental/private/ndbuffer_image.hpp>


namespace mln::experimental
{
  template <class T>
  using image1d = mln::__ndbuffer_image<T, 1>;

  template <class T>
  using image2d = mln::__ndbuffer_image<T, 2>;

  template <class T>
  using image3d = mln::__ndbuffer_image<T, 3>;
}
