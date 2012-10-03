#ifndef MLN_CORE_CONCEPT_CHECK_HPP
# define MLN_CORE_CONCEPT_CHECK_HPP

# include <type_traits>
# include <string>

namespace internal
{
  template <bool cond, typename T>
  struct mln_check_if_
  {
    mln_check_if_() {}
  };

  template <typename T>
  struct mln_check_if_<true, T>
  {
    mln_check_if_() { ((T*)0)->~T(); }
  };
}

# define MLN_CONCEPT_BEGIN_CHECK_IF()					\
  {									\
  struct __mln_check_if__						\
  {									\
  ~__mln_check_if__()							\
  {									\


# define MLN_CONCEPT_END_CHECK_IF(TEST)			\
  }							\
    };							\
  internal::mln_check_if_<TEST, __mln_check_if__> ();	\
  }




namespace mln
{

  void check(std::true_type)
  {
  }

  void check(std::true_type, std::string)
  {
  }

  void check_false(std::false_type)
  {
  }

  void check_false(std::false_type, std::string)
  {
  }


} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_CHECK_HPP
