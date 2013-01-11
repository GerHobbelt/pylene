#ifndef MLN_CORE_IMAGE_IMAGE2D_HPP
# define MLN_CORE_IMAGE_IMAGE2D_HPP

# include <mln/core/image/ndimage.hpp>
# include <mln/core/ch_value.hpp>

namespace mln {

  ///
  /// \brief The standard 2D image type.
  ///
  /// \p image2d is a type to represent 2d image. They are implemented with contiguous
  /// data buffer to allow pointer arithmetic. However data are not contiguous in this buffer
  /// since each row is aligned on a 128-bits boundary to get performance. \p image2d are soft image
  /// since two images can share the same buffer of data to avoid uncessary copies.
  ///
  /// \p image2d models a Writable Point-Accessible Image concept.
  /// See \see image2d \see image3d
  template <typename T>
  using image2d = ndimage<T, 2>;


} // end of namespace mln



#endif //!MLN_CORE_IMAGE_IMAGE2D_HPP
