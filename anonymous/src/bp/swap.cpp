#include <ano/bp/swap.hpp>

#include <cstddef>
#include <cstring>


namespace ano::bp::impl
{

  void swap_raw(std::byte* __restrict a, std::byte* __restrict b, std::size_t len, int height, std::ptrdiff_t a_stride,
                std::ptrdiff_t b_stride) noexcept
  {
    static constexpr int  BLOCK_SIZE      = 32;
    static constexpr int  HALF_BLOCK_SIZE = 16;
    alignas(32) std::byte tmp[BLOCK_SIZE];

    int rem0 = len % BLOCK_SIZE;
    int rem1 = len % HALF_BLOCK_SIZE;

    for (int y = 0; y < height; ++y)
    {
      auto a_ptr = a + y * a_stride;
      auto b_ptr = b + y * b_stride;

      std::size_t x;
      for (x = 0; x + BLOCK_SIZE <= len; x += BLOCK_SIZE)
        swap_buffer(a_ptr + x, b_ptr + x, tmp, BLOCK_SIZE);

      if (rem0 > HALF_BLOCK_SIZE)
      {
        swap_buffer(a_ptr + x, b_ptr + x, tmp, HALF_BLOCK_SIZE);
        x += HALF_BLOCK_SIZE;
      }

      if (rem1 > 0)
        swap_buffer(a_ptr + x, b_ptr + x, tmp, rem1);
    }
  }
} // namespace ano::bp::impl
