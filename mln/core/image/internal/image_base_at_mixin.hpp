#ifndef MLN_CORE_IMAGE_INTERNAL_IMAGE_BASE_AT_MIXIN_HPP
# define MLN_CORE_IMAGE_INTERNAL_IMAGE_BASE_AT_MIXIN_HPP

# include <mln/core/point.hpp>

namespace mln
{

  namespace internal
  {

    template <typename Derived, typename Point, typename Reference, typename ConstReference, typename Enable = void>
    struct image_base_at_mixin
    {
    };


    template <typename Derived, typename T, typename Reference, typename ConstReference>
    struct image_base_at_mixin<Derived, point<T, 2>, Reference, ConstReference, typename std::enable_if< image_traits<Derived>::accessible::value >::type>
    {

      Reference at(T coord0, T coord1) { return this->derived()->at(point<T,2>{coord0, coord1}); }
      ConstReference at(T coord0, T coord1) const { return this->derived()->at(point<T,2>{coord0, coord1}); }

      Reference operator() (T coord0, T coord1) { return (*this->derived())(point<T,2>{coord0, coord1}); }
      ConstReference operator()(T coord0, T coord1) const { return (*this->derived())(point<T,2>{coord0, coord1}); }


    private:
      Derived* derived() { return reinterpret_cast<Derived*>(this); }
      const Derived* derived() const { return reinterpret_cast<const Derived*>(this); }
    };


    template <typename Derived, typename T, typename Reference, typename ConstReference>
    struct image_base_at_mixin<Derived, point<T, 3>, Reference, ConstReference, typename std::enable_if< image_traits<Derived>::accessible::value >::type>
    {

      Reference at(T coord0, T coord1, T coord2) { return derived()->at(point<T,3>{coord0, coord1, coord2}); }
      ConstReference at(T coord0, T coord1, T coord2) const { return derived()->at(point<T,3>{coord0, coord1, coord2}); }

      Reference operator() (T coord0, T coord1, T coord2) { return (*this->derived())(point<T,3>{coord0, coord1, coord2}); }
      ConstReference operator()(T coord0, T coord1, T coord2) const { return (*this->derived())(point<T,3>{coord0, coord1, coord2}); }

    private:
      Derived* derived() { return reinterpret_cast<Derived*>(this); }
      const Derived* derived() const { return reinterpret_cast<const Derived*>(this); }
    };



  }

}

#endif // ! MLN_CORE_IMAGE_INTERNAL_IMAGE_BASE_AT_MIXIN_HPP
