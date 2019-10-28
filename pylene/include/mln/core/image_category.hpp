#pragma once

namespace mln
{
  // clang-format off
  struct forward_image_tag
  {
  };
  struct bidirectional_image_tag : forward_image_tag
  {
  };
  struct[[deprecated]] random_access_image_tag : bidirectional_image_tag
  {
  };
  struct raw_image_tag : random_access_image_tag
  {
  };
  //clang-format on

} // namespace mln
