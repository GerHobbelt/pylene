#include <mln/core/image/private/ndbuffer_image_data.hpp>

#include <pln/core/image_cast.hpp>

#include <pybind11/pybind11.h>

#include <memory>

namespace pln
{
  namespace py = pybind11;

  PYBIND11_MODULE(pylena, m)
  {
    init_pylena_numpy(m);
  }
} // namespace pln