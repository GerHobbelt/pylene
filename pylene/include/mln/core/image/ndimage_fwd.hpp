#pragma once


namespace mln
{
  template <class T, int dim>
  class __ndbuffer_image;

  using ndbuffer_image = __ndbuffer_image<void, -1>;

  template <class T>
  using image1d = mln::__ndbuffer_image<T, 1>;

  template <class T>
  using image2d = mln::__ndbuffer_image<T, 2>;

  template <class T>
  using image3d = mln::__ndbuffer_image<T, 3>;
}
