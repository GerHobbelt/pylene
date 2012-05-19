#ifndef MLN_CORE_IMAGE_BASE_HPP
# define MLN_CORE_IMAGE_BASE_HPP

# include <cstddef>

namespace mln
{


    template <typename Point,
              typename Value,
              typename Reference = Value&,
              typename ConstReference = const Value&,
              typename Pointer = Value*,
              typename ConstPointer = const Value*,
              typename SizeType = std::size_t,
              typename DifferenceType = std::ptrdiff_t
              >
    struct image_base
    {
      typedef Point point_type;
      typedef Point site_type;

      typedef Value value_type;
      typedef Reference reference;
      typedef ConstReference const_reference;
      typedef Pointer pointer;

      typedef SizeType size_type;
      typedef DifferenceType difference_type;
    };

} // end of namespace mln


#endif //!MLN_CORE_IMAGE_BASE_HPP
