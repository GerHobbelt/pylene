#pragma once

#include <boost/concept_check.hpp>

#include <string>
#include <type_traits>


#define ANO_CONCEPT_ASSERT_IF(TEST, CONCEPT) ano::internal::ano_check_if_<TEST, CONCEPT>();

namespace ano
{

  namespace internal
  {
    template <bool cond, typename T>
    struct ano_check_if_
    {
      ano_check_if_() {}
    };

    template <typename T>
    struct ano_check_if_<true, T>
    {
      ano_check_if_() { BOOST_CONCEPT_ASSERT((T)); }
    };
  } // namespace internal

  template <bool condition>
  using check_t = std::integral_constant<bool, condition>;

  inline void check(std::true_type) {}

  inline void check(std::true_type, std::string) {}

  inline void check_false(std::false_type) {}

  inline void check_false(std::false_type, std::string) {}

  template <typename T>
  T&& make_object()
  {
    return static_cast<T&&>(*(typename std::remove_reference<T>::type*)(0));
  }

} // namespace ano
