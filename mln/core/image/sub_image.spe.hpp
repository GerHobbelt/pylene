#ifndef MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP
# define MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP

namespace mln
{

  // Specialization for ndimage | box
  // fwd decl
  template <typename, unsigned, typename> struct ndimage_base;

  template <typename T, unsigned dim, typename E>
  const E make_subimage(const ndimage_base<T, dim, E>&,
                        const typename ndimage_base<T, dim, E>::domain_type& domain);

  template <typename T, unsigned dim, typename E>
  E make_subimage(ndimage_base<T, dim, E>&,
                  const typename ndimage_base<T, dim, E>::domain_type& domain);

  template <typename T, unsigned dim, typename E>
  E make_subimage(ndimage_base<T, dim, E>&&,
                  const typename ndimage_base<T, dim, E>::domain_type& domain);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename T, unsigned dim, typename E>
  inline
  E
  make_subimage(ndimage_base<T, dim, E>& image,
                const typename ndimage_base<T, dim, E>::domain_type& domain)
  {
    E other(exact(image));
    other.domain_ = domain;
    other.border_ = 0;
    other.ptr_ = (char*) &image(domain.pmin);
    other.last_ = (char*) &image(domain.pmax - 1);
    return other;
  }

  template <typename T, unsigned dim, typename E>
  inline
  const E
  make_subimage(const ndimage_base<T, dim, E>& image,
                const typename ndimage_base<T, dim, E>::domain_type& domain)
  {
    return make_subimage(const_cast< ndimage_base<T, dim, E>& >(image), domain);
  }


  template <typename T, unsigned dim, typename E>
  inline
  E
  make_subimage(ndimage_base<T, dim, E>&& image,
                const typename ndimage_base<T, dim, E>::domain_type& domain)
  {
    return make_subimage(image, domain);
  }


} // end of namespace mln

#endif //!MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP
