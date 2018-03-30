using uint8 = unsigned char;

#if __cpp_coroutines >= 201703L

# include "generator.hpp"
# include <experimental/coroutine>
using cppcoro::generator;

namespace
{
  generator<uint8*> viter(uint8* buffer, std::size_t sz) noexcept
  {
    for (std::size_t x = 0; x < sz; ++x)
    {
      co_yield buffer + x;
    }
  }
}


void Mult_Inplace_Coro(std::ptrdiff_t*  info, uint8* buffer)
{
  int nrow = info[0];
  int ncol = info[1];
  //std::ptrdiff_t stride = info[2];

  std::size_t s = nrow * ncol;
  generator<uint8*> gen = viter(buffer, s);
  for (auto ptr : gen)
  {
    *ptr *= 2;
  }
}

#else

# include <cstddef>

void Mult_Inplace_Coro(std::ptrdiff_t*, uint8*)
{
}

#endif
