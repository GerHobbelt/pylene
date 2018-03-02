#ifndef MLN_CORE_UTILS_PTROFFSET_HPP
#define MLN_CORE_UTILS_PTROFFSET_HPP

#include <cstddef>

namespace mln
{

  /// \brief Move the pointer to \p offset *bytes*.
  template <class T>
  T* ptr_offset(T* x, std::ptrdiff_t offset)
  {
    return (T*)(((char*)x) + offset);
  }


  template <class T>
  T* ptr_offset(std::uintptr_t x, std::ptrdiff_t offset)
  {
    return reinterpret_cast<T*>(x + offset);
  }

} // end of namespace mln

#endif //! MLN_CORE_UTILS_PTROFFSET_HPP
