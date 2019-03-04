#pragma once

#include <type_traits>


namespace mln
{

  // Specialization for ndimage | box
  // fwd decl
  template <typename, unsigned, typename>
  struct ndimage_base;


  template <typename T, unsigned dim, typename E, typename Domain>
  std::enable_if_t<std::is_convertible<Domain, typename ndimage_base<T, dim, E>::domain_type>::value, const E>
      make_subimage(const ndimage_base<T, dim, E>&, const Domain& domain);


  template <typename T_, unsigned dim_, typename E_, typename Domain_>
  std::enable_if_t<std::is_convertible<Domain_, typename ndimage_base<T_, dim_, E_>::domain_type>::value, E_>
      make_subimage(ndimage_base<T_, dim_, E_>&, const Domain_& domain);


  template <typename T, unsigned dim, typename E, typename Domain>
  std::enable_if_t<std::is_convertible<Domain, typename ndimage_base<T, dim, E>::domain_type>::value, E>
      make_subimage(ndimage_base<T, dim, E>&&, const Domain& domain);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename T_, unsigned dim_, typename E_, typename Domain_>
  std::enable_if_t<std::is_convertible<Domain_, typename ndimage_base<T_, dim_, E_>::domain_type>::value, E_>
      make_subimage(ndimage_base<T_, dim_, E_>& image, const Domain_& domain)
  {
    return image.clip(domain);
  }


  template <typename T, unsigned dim, typename E, typename Domain>
  std::enable_if_t<std::is_convertible<Domain, typename ndimage_base<T, dim, E>::domain_type>::value, const E>
      make_subimage(const ndimage_base<T, dim, E>& image, const Domain& domain)
  {
    return image.clip(domain);
  }


  template <typename T, unsigned dim, typename E, typename Domain>
  std::enable_if_t<std::is_convertible<Domain, typename ndimage_base<T, dim, E>::domain_type>::value, E>
      make_subimage(ndimage_base<T, dim, E>&& image, const Domain& domain)
  {
    return image.clip(domain);
  }

} // namespace mln
