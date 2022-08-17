#pragma once


namespace ano
{
  template <class T, int dim>
  class __ndbuffer_image;

  using ndbuffer_image = __ndbuffer_image<void, -1>;

  template <class T>
  using image1d = ano::__ndbuffer_image<T, 1>;

  template <class T>
  using image2d = ano::__ndbuffer_image<T, 2>;

  template <class T>
  using image3d = ano::__ndbuffer_image<T, 3>;


  template <class T, int dim>
  using ndimage = ano::__ndbuffer_image<T, dim>;
} // namespace ano
