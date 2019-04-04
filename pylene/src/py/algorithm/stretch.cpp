#include <py/algorithm/stretch.hpp>

image2d<void> stretch(const image2d<>& src)
{
  switch (src.type().val)
  {
  case (Info::INT8_V):
    return stretch(static_cast<const image2d<int8_t>&>(src));
  case (Info::INT16_V):
    return stretch(static_cast<const image2d<int16_t>&>(src));
  default:
    throw std::runtime_error("Data type not implemented");
  }
}