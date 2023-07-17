#include <pln/core/image_cast.hpp>

#include <pybind11/stl.h>

#include <memory>

#include "scribo/scribo.hpp"

namespace pln
{
  namespace py = pybind11;

  PYBIND11_MODULE(pylena, m)
  {
    init_pylena_numpy(m);

#ifdef ENABLE_SCRIBO
    pln::scribo::define_scribo(m);
#endif
  }
} // namespace pln