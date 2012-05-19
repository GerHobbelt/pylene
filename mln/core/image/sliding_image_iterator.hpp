#ifndef MLN_CORE_SLIDING_IMAGE_ITERATOR_HPP
# define MLN_CORE_SLIDING_IMAGE_ITERATOR_HPP

# include <mln/core/pixel.hpp>
# include <boost/iterator/iterator_facade.hpp>

namespace mln {


  template <typename Image, typename SiteSet, typename image_tag = typename image_traits<Image>::category>
  struct sliding_image_value_iterator;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/



  template <typename Image, size_t N>
  struct sliding_image_value_iterator<Image, std::array<typename Image::point_type, N>, raw_image_tag>
    :  boost::iterator_facade< sliding_image_value_iterator<Image,
                                                            std::array<typename Image::point_type, N>,
                                                            raw_image_tag>, // Exact
                               typename std::conditional<std::is_const<Image>::value,
                                                         typename std::add_const<typename Image::value_type>::type,
                                                         typename Image::value_type>::type, // Value type
                               boost::bidirectional_traversal_tag >
  {
  private:
    typedef typename std::conditional<std::is_const<Image>::value,
                                      typename std::add_const<typename Image::value_type>::type,
                                      typename Image::value_type>::type V;

  public:
  typedef typename Image::point_type P;
  typedef typename Image::difference_type difference_type;


  public:

    sliding_image_value_iterator() {};
    sliding_image_value_iterator(V* ptr, const std::array<difference_type, N>& offsets, int i)
      : offsets_ (offsets), ptr_ ((char*)ptr),  i_ (i)
    {
    }


  private:
    typedef sliding_image_value_iterator<Image, std::array<P, N>, raw_image_tag> this_t;
    friend class boost::iterator_core_access;

    V& dereference() const { return *(V*)(ptr_ + offsets_[i_]); }
    void increment() { ++i_; }
    void decrement() { --i_; }


    template <typename I2>
    typename std::enable_if<std::is_same<typename std::remove_cv<I2>::type,
                                         typename std::remove_cv<this_t>::type>::value, bool>::type
    equal(const sliding_image_value_iterator<I2, std::array<P, N>, raw_image_tag>& other) const
    {
      return i_ == other.i_;
    }


    const std::array<difference_type, N>& offsets_;
    char*                        ptr_;
    int                       i_;
  };

}

#endif //!MLN_CORE_SLIDING_IMAGE_ITERATOR_HPP
