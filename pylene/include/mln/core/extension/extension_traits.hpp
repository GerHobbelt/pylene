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

    namespace experimental
    {
      enum class Pattern
      {
        Mirror,
        Periodize,
        Clamp
      };
    } // namespace experimental
  }   // namespace extension

  /******************************************/
  /****              Traits              ****/
  /******************************************/

  template <typename E>
  struct extension_traits
  {
    typedef typename E::support_fill      support_fill;
    typedef typename E::support_mirror    support_mirror;
    typedef typename E::support_periodize support_periodize;
  };
} // namespace mln
