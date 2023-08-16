#pragma once


namespace mln
{
  namespace extension
  {

    /******************************************/
    /****               Tags               ****/
    /******************************************/

    struct extension_tag
    {
    };
    struct none_extension_tag : extension_tag
    {
    };
    struct custom_extension_tag : extension_tag
    {
    };
    struct border_extension_tag : custom_extension_tag
    {
    };
    struct value_extension_tag : custom_extension_tag
    {
    };
    struct image_extension_tag : custom_extension_tag
    {
    };
  } // namespace extension

  /******************************************/
  /****              Traits              ****/
  /******************************************/

  template <typename E>
  struct extension_traits
  {
    using support_fill        = typename E::support_fill;
    using support_mirror      = typename E::support_mirror;
    using support_periodize   = typename E::support_periodize;
    using support_clamp       = typename E::support_clamp;
    using support_extend_with = typename E::support_extend_with;
  };
} // namespace mln
