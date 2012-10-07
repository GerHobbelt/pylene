#ifndef MLN_CORE_IMAGE_SUB_IMAGE_HPP
# define MLN_CORE_IMAGE_SUB_IMAGE_HPP

# include <type_traits>

# include <mln/core/image_base.hpp>
# include <mln/core/range/range_traits.hpp>

namespace mln
{

  template <typename Image, typename Domain>
  struct sub_image;

  /// These overloads are worst-conversion possible
  /// \{
  template <typename I, typename Domain>
  sub_image<const I&, Domain>
  make_subimage(const Image<I>& ima, const Domain& domain);

  template <typename I, typename Domain>
  sub_image<I&, Domain>
  make_subimage(Image<I>& ima, const Domain& domain);

  template <typename I, typename Domain>
  sub_image<const I, Domain>
  make_subimage(const Image<I>&& ima, const Domain& domain);

  template <typename I, typename Domain>
  sub_image<I, Domain>
  make_subimage(Image<I>&& ima, const Domain& domain);
  /// \}

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<const I&>(), std::declval<const Domain&>()) )
  operator| (const Image<I>& ima, const Domain& domain);

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<I&>(), std::declval<const Domain&>()) )
  operator| (Image<I>& ima, const Domain& domain);

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<I&&>(), std::declval<const Domain&>()) )
  operator| (const Image<I>&& ima, const Domain& domain);

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<I&&>(), std::declval<const Domain&>()) )
  operator| (Image<I>&& ima, const Domain& domain);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  /// These overloads are worst-conversion possible
  /// \{
  template <typename I, typename Domain>
  sub_image<const I&, Domain>
  make_subimage(const Image<I>& ima, const Domain& domain)
  {
    return sub_image<const I&, Domain>(exact(ima), domain);
  }

  template <typename I, typename Domain>
  sub_image<I&, Domain>
  make_subimage(Image<I>& ima, const Domain& domain)
  {
    return sub_image<I&, Domain>(exact(ima), domain);
  }

  template <typename I, typename Domain>
  sub_image<const I, Domain>
  make_subimage(const Image<I>&& ima, const Domain& domain)
  {
    return sub_image<I, Domain>(move_exact(ima), domain);
  }

  template <typename I, typename Domain>
  sub_image<I, Domain>
  make_subimage(Image<I>&& ima, const Domain& domain)
  {
    return sub_image<I, Domain>(move_exact(ima), domain);
  }

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<const I&>(), std::declval<const Domain&>()) )
  operator| (const Image<I>& ima, const Domain& domain)
  {
    return make_subimage(exact(ima), domain);
  }

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<I&>(), std::declval<const Domain&>()) )
  operator| (Image<I>& ima, const Domain& domain)
  {
    return make_subimage(exact(ima), domain);
  }

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<I&&>(), std::declval<const Domain&>()) )
  operator| (const Image<I>&& ima, const Domain& domain)
  {
    return make_subimage(move_exact(ima), domain);
  }

  template <typename I, typename Domain>
  decltype( make_subimage(std::declval<I&&>(), std::declval<const Domain&>()) )
  operator| (Image<I>&& ima, const Domain& domain)
  {
    return make_subimage(move_exact(ima), domain);
  }



  /******************************************/
  /****              Traits              ****/
  /******************************************/


  template <typename Image, typename Domain>
  struct image_traits< sub_image<Image, Domain> >
  {
    typedef std::true_type      accessible;
    typedef forward_image_tag   category; // FIXME: category depends on domain category
  };



  template <typename I, typename Domain>
  struct sub_image : image_base< sub_image<I, Domain>,
                                 typename Domain::point_type,
                                 typename image_value<typename std::remove_reference<I>::type>::type >
  {
    BOOST_CONCEPT_ASSERT((AccessibleImage<typename std::decay<I>::type>));
  private:
    typedef typename std::remove_reference<I>::type  image_t;

  public:
    typedef typename range_value<Domain>::type                  point_type;
    typedef typename image_value<image_t>::type                 value_type;
    typedef typename image_reference<image_t>::type             reference;
    typedef typename image_const_reference<image_t>::type       const_reference;

    // typedef restricted_value_iterator<image_t, typename Domain::iterator>             value_iterator;
    // typedef restricted_value_iterator<const image_t, typename Domain::iterator>       const_value_iterator;
    // typedef restricted_pixel_iterator<image_t, typename Domain::iterator>             pixel_iterator;
    // typedef restricted_pixel_iterator<const image_t, typename Domain::iterator>       const_pixel_iterator;

    // typedef typename pixel_iterator::value_type          pixel_type;
    // typedef typename const_pixel_iterator::value_type    const_pixel_type;


    sub_image(I&& ima, const Domain& domain);




  private:
    I           m_ima;
    Domain      m_domain;
  };



} // end of namespace mln

# include <mln/core/image/sub_image.spe.hpp>

#endif //!MLN_CORE_IMAGE_SUB_IMAGE_HPP
