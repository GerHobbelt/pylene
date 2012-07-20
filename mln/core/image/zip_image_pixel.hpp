#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP

namespace mln {

  template <typename... Images>
  struct zip_image_pixel;

  template <typename... Images>
  struct zip_image_const_pixel;


/******************************************/
/****          Implementation          ****/
/******************************************/

  template <typename... Images>
  struct zip_image_pixel
  {
  private:
    typedef zip_image<Images...> I;
    typedef typename I::value_type V;
    typedef typename I::reference R;
    typedef typename I::point_type P;

  public:
    zip_image_pixel(zip_image<Images...>* ima);

    P point() const;
    R val() const;
    I& image() const;

  private:
    zip_image<Images...>* const ima_;
    typename I::domain_type::iterator pit;
    type
  };

  template <typename... Images>
  struct zip_image_const_pixel
  {
  private:
    typedef zip_image<Images...> I;
    typedef typename I::value_type V;
    typedef typename I::const_reference R;
    typedef typename I::point_type P;

  public:
    P point() const;
    R val() const;
    const I& image() const;

  private:
    const zip_image<Images...>* const ima_;
  };



} // end of namespace mln


#endif //!MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP
