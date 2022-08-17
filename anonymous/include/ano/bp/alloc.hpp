#pragma once
#include <ano/bp/utils.hpp>
#include <cstddef>
#include <memory>

namespace ano::bp
{

  /// \brief Return an unitialized 2D-buffer with lines aligned on 32 bytes boundaries.
  ///
  /// The pitch is the number of bytes between two consecutive lines. The pointer as to be freed
  /// with ``mlb::bp::free``
  ///
  ///
  /// \rst
  /// :param width: Width of the 2D-buffer
  /// :param height: Height of the 2D-buffer
  /// :param out pitch: The number of bytes between two lines
  /// :return: Pointer to the allocated buffer
  /// \endrst
  template <class T>
  [[gnu::assume_aligned(32)]] T* aligned_alloc_2d(int width, int height, std::ptrdiff_t& pitch);

  [[gnu::assume_aligned(32)]] void* aligned_alloc_2d(int width, int height, std::size_t esize, std::ptrdiff_t& pitch);


  /// \brief Frees a 2D buffer allocated with ``ano::bp::aligned_alloc_2d``
  ///
  /// \rst
  /// :param ptr: Pointer to the 2D-buffer
  /// \endrst
  void aligned_free_2d(void* ptr);

  template <class T>
  void aligned_free_2d(T* ptr, int width, int height, std::ptrdiff_t pitch);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T>
  [[gnu::assume_aligned(32)]] T* aligned_alloc_2d(int width, int height, std::ptrdiff_t& pitch)
  {
    T* buffer = (T*)aligned_alloc_2d(width, height, sizeof(T), pitch);

    std::ptrdiff_t s = pitch;
    for (int y = 0; y < height; y++)
      std::uninitialized_default_construct_n(ano::bp::ptr_offset(buffer, s * y), width);

    return buffer;
  }

  template <class T>
  inline void aligned_free_2d(T* ptr, int width, int height, std::ptrdiff_t pitch)
  {
    for (int y = 0; y < height; y++)
      std::destroy_n(ano::bp::ptr_offset(ptr, pitch * y), width);

    aligned_free_2d((void*)ptr);
  }


} // namespace ano::bp
