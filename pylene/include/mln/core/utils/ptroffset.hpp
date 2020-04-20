#pragma once

#include <cstddef>


namespace mln
{

  /// \brief Move the pointer to \p offset *bytes*.
  template <class T>
  T* ptr_offset(T* x, std::ptrdiff_t offset)
  {
    return (T*)(((char*)x) + offset);
  }

} // namespace mln
