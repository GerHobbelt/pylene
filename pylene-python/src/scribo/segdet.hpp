#pragma once

#include <mln/core/image/ndbuffer_image.hpp>

#include <pybind11/stl.h>

#include <map>
#include <string>

namespace pln::scribo
{


  mln::ndbuffer_image detect_line_label(mln::ndbuffer_image img, int min_len,
                                        const std::map<std::string, float>& params = std::map<std::string, float>());

  mln::ndbuffer_image detect_line(mln::ndbuffer_image img, int min_len, const std::string& mode = "pixel",
                                  const std::map<std::string, float>& params = std::map<std::string, float>());
} // namespace pln::scribo