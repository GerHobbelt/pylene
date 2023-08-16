#pragma once

#include <mln/core/config.hpp>

#include <type_traits>


namespace mln
{

  template <typename E>
  struct Object;

  template <typename E>
  E& exact(Object<E>& object);
  template <typename E>
  const E& exact(const Object<E>& object);
  template <typename E>
  E* exact(Object<E>* object);
  template <typename E>
  const E* exact(const Object<E>* object);
  template <typename E>
  E&& exact(Object<E>&& object);
  template <typename E>
  const E&& exact(const Object<E>&& object);
  template <typename E>
  E&& move_exact(Object<E>& object);
  template <typename E>
  const E&& move_exact(const Object<E>& object);

  namespace internal
  {

    template <typename T, template <typename> class Concept>
    struct is_a_helper
    {
      template <typename dummy>
      static std::true_type foo(const Concept<dummy>&)
      {
        return std::true_type();
      }

      static std::false_type foo(...) { return std::false_type(); }

      using type = decltype(foo(std::declval<T>()));
    };
  } // namespace internal

  template <typename T, template <typename> class Concept>
  using is_a = typename internal::is_a_helper<T, Concept>::type;


  template <typename T, template <typename> class Concept>
  constexpr inline bool is_a_v = is_a<T, Concept>::value;

  /*********************/
  /* Implementation    */
  /*********************/

  template <typename E>
  inline E& exact(Object<E>& object)
  {
    return *static_cast<E*>(&object);
  }

  template <typename E>
  inline const E& exact(const Object<E>& object)
  {
    return *static_cast<const E*>(&object);
  }

  template <typename E>
  inline E* exact(Object<E>* object)
  {
    return static_cast<E*>(object);
  }

  template <typename E>
  inline const E* exact(const Object<E>* object)
  {
    return static_cast<const E*>(object);
  }

  template <typename E>
  inline E&& exact(Object<E>&& object)
  {
    return static_cast<E&&>(object);
  }

  template <typename E>
  inline E&& move_exact(Object<E>& object)
  {
    return static_cast<E&&>(object);
  }

  template <typename E>
  inline const E&& move_exact(const Object<E>& object)
  {
    return static_cast<E&&>(object);
  }

  template <typename E>
  struct Object
  {
    using exact_type = E;

    E*       exact() { return static_cast<E*>(this); }
    const E* exact() const { return static_cast<const E*>(this); }

  protected:
    Object(const Object&)            = default;
    Object(Object&&)                 = default;
    Object()                         = default;
    Object& operator=(Object&&)      = default;
    Object& operator=(const Object&) = default;
  };

  template <typename E>
  struct Object_ : Object<E>
  {

  public:
    Object_(const Object_&) = default;
    // Object_ (Object_&&) = default;
    Object_() = default;
  };

  template <typename E>
  struct exact_type
  {
    using type = typename E::exact_type;
  };

  template <typename E>
  struct exact_type<const E>
  {
    using type = const typename E::exact_type;
  };

} // namespace mln
