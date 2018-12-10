#ifndef MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP
#define MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP

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
    E_ other(exact(image));
    other.m_domain      = domain;
    other.m_border      = image.m_border; // FIXME
    other.m_ptr         = (char*)&image(domain.pmin);
    other.m_last        = (char*)&image(typename Domain_::point_type(domain.pmax - 1));
    other.m_ptr_origin  = image.m_ptr_origin;
    other.m_index_first = image.index_of_point(domain.pmin);
    other.m_index_last  = image.index_of_point(domain.pmax - 1);
    return other;
  }


  template <typename T, unsigned dim, typename E, typename Domain>
  std::enable_if_t<std::is_convertible<Domain, typename ndimage_base<T, dim, E>::domain_type>::value, const E>
      make_subimage(const ndimage_base<T, dim, E>& image, const Domain& domain)
  {
    return make_subimage(const_cast<ndimage_base<T, dim, E>&>(image), domain);
  }


  template <typename T, unsigned dim, typename E, typename Domain>
  std::enable_if_t<std::is_convertible<Domain, typename ndimage_base<T, dim, E>::domain_type>::value, E>
      make_subimage(ndimage_base<T, dim, E>&& image, const Domain& domain)
  {
    return make_subimage(image, domain);
  }

} // end of namespace mln

#endif //! MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP
