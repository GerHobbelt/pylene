#ifndef MLN_CORE_CONCEPT_IMAGE_HPP
# define MLN_CORE_CONCEPT_IMAGE_HPP

# include <mln/core/concept/object.hpp>

namespace mln {

  template <typename I>
  struct Image : Object_<I>
  {
  };


} // end of namespace mln


#endif //!MLN_CORE_CONCEPT_IMAGE_HPP
