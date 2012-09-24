#ifndef MLN_CORE_CONCEPT_CHECK_HPP
# define MLN_CORE_CONCEPT_CHECK_HPP

namespace mln
{

  void check(std::true_type)
  {
  }

  void check(std::true_type, std::string)
  {
  }


} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_CHECK_HPP
