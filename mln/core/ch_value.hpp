#ifndef MLN_CORE_CH_VALUE_HPP
# define MLN_CORE_CH_VALUE_HPP

# define mln_ch_value(I, V) typename image_ch_value<I, V>::type
# define mln_concrete(I)    typename image_concrete<I>::type

namespace mln
{

  template <typename Domain>
  struct image_from_domain;

  template <typename Image>
  struct image_concrete : image_from_domain<typename Image::domain_type>
  {
  };

  // If no specialization get concrete image from domain
  template <typename Image, typename V>
  struct image_ch_value : image_ch_value<typename image_concrete<Image>::type, V>
  {
  };

}

#endif // !MLN_CORE_CH_VALUE_HPP
