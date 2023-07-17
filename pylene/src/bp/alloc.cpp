#include <mln/bp/alloc.hpp>
#include <cstdlib>

namespace mln::bp
{


  void* aligned_alloc_2d(int width, int height, std::size_t e, std::ptrdiff_t& pitch)
  {
    pitch = (width * e + 0x1F) & 0xFFFFFFE0;
#ifdef _MSC_VER
    return _aligned_malloc(pitch * height, 32);
#else
      return std::aligned_alloc(32, pitch * height);
#endif
  }


  void aligned_free_2d(void* ptr)
  {
#ifdef _MSC_VER
    return _aligned_free(ptr);
#else
    return std::free(ptr);
#endif
  }

}
