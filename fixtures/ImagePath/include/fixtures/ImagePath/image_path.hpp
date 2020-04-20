#pragma once

#include <string_view>

namespace fixtures::ImagePath
{

  std::string_view get_image_path();
  std::string      concat_with_filename(std::string_view filename);

} // namespace fixtures::ImagePath