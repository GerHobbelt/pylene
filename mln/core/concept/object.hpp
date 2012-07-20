#ifndef MLN_CORE_CONCEPT_OBJECT_HPP
# define MLN_CORE_CONCEPT_OBJECT_HPP


namespace mln {

  template <typename E>
  struct Object;

  template <typename E>
  inline
  E& exact(Object<E>& object)
  {
    return *static_cast<E*>(&object);
  }

  template <typename E>
  inline
  const E& exact(const Object<E>& object)
  {
    return *static_cast<const E*>(&object);
  }

  template <typename E>
  inline
  E* exact(Object<E>* object)
  {
    return static_cast<E*>(object);
  }

  template <typename E>
  inline
  const E* exact(const Object<E>* object)
  {
    return static_cast<const E*>(object);
  }


  template <typename E>
  inline
  E&& exact(Object<E>&& object)
  {
    return static_cast<E&&>(object);
  }

  template <typename E>
  inline
  E&& fwd_exact(Object<E>& object)
  {
    return static_cast<E&&>(object);
  }

  template <typename E>
  inline
  E&& fwd_exact(const Object<E>& object)
  {
    return static_cast<E&&>(object);
  }


  template <typename E>
  struct Object
  {

  protected:
    Object (const Object&) = default;
    //Object (Object&&) = default;
    Object () = default;
  };

  template <typename E>
  struct Object_ : Object<E>
  {

  public:
    Object_ (const Object_&) = default;
    //Object_ (Object_&&) = default;
    Object_ () = default;
  };



} // end of namespace mln


#endif //!MLN_CORE_CONCEPT_OBJECT_HPP
