#ifndef MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP
# define MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP

# include <type_traits>

namespace mln
{

  // Specialization for ndimage | box
  // fwd decl
  template <typename, unsigned, typename> struct ndimage_base;

  template <typename T, unsigned dim, typename Domain>
  typename std::enable_if< std::is_convertible<Domain, typename ndimage<T, dim>::domain_type>::value, ndimage<T, dim> >::type
  make_subimage(const ndimage<T, dim>&,
		const Domain& domain);

  template <typename T, unsigned dim, typename Domain>
  typename std::enable_if< std::is_convertible<Domain, typename ndimage<T, dim>::domain_type>::value, ndimage<T, dim> >::type
  make_subimage(ndimage<T, dim>&,
		const Domain& domain);

  template <typename T, unsigned dim, typename Domain>
  typename std::enable_if< std::is_convertible<Domain, typename ndimage<T, dim>::domain_type>::value, ndimage<T, dim> >::type
  make_subimage(ndimage<T, dim>&&,
		const Domain& domain);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename T, unsigned dim, typename Domain>
  inline
  typename std::enable_if< std::is_convertible<Domain, typename ndimage<T, dim>::domain_type>::value, ndimage<T, dim> >::type
  make_subimage(ndimage<T, dim>& image,
                const Domain& domain)
  {
    return make_subimage((const ndimage<T, dim>&) image, domain);
  }

  template <typename T, unsigned dim, typename Domain>
  inline
  typename std::enable_if< std::is_convertible<Domain, typename ndimage<T, dim>::domain_type>::value, ndimage<T, dim> >::type
  make_subimage(const ndimage<T, dim>& image,
                const Domain& domain)
  {
    ndimage<T, dim> other(image);
    other.domain_ = domain;
    other.border_ = 0;
    other.ptr_ = (char*) &image(domain.pmin);
    other.last_ = (char*) &image(domain.pmax - 1);
    other.m_ptr_origin = image.m_ptr_origin;
    other.m_index_first = image.index_of_point(domain.pmin);
    other.m_index_last = image.index_of_point(domain.pmax);
    return other;
  }


  template <typename T, unsigned dim, typename Domain>
  inline
  typename std::enable_if< std::is_convertible<Domain, typename ndimage<T, dim>::domain_type>::value, ndimage<T, dim> >::type
  make_subimage(ndimage<T, dim>&& image,
                const Domain& domain)
  {
    return make_subimage(image, domain);
  }


} // end of namespace mln

#endif //!MLN_CORE_IMAGE_SUB_IMAGE_SPE_HPP
