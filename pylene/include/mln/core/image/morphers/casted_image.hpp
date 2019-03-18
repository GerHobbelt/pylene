#pragma once

#include <mln/core/image/morphers/transformed_image.hpp>
#include <mln/core/image/view/transform.hpp>


namespace mln
{
  // Exposition only
  namespace internal
  {
    template <typename Vout>
    struct cast_to;
  }
  // End

  template <class I, class V>
  using casted_image = internal::transformed_image<I, internal::cast_to<V>, false>;

  template <class V, class I>
  casted_image<const I&, V> imcast(const Image<I>& ima);

  template <class V, class I>
  casted_image<I&, V> imcast(Image<I>& ima);

  template <class V, class I>
  casted_image<I, V> imcast(Image<I>&& ima);

  namespace experimental
  {
    template <class I, class V>
    using casted_image = transform_view<I, internal::cast_to<V>>;

    template <class V, class IntputImage>
    casted_image<IntputImage, V> imcast(IntputImage ima);
  } // namespace experimental

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {
    template <typename V>
    struct cast_to
    {
      template <typename T>
      V operator()(const T& v) const
      {
        return static_cast<V>(v);
      }
    };
  } // namespace internal

  template <class V, class I>
  casted_image<const I&, V> imcast(const Image<I>& ima)
  {
    return casted_image<const I&, V>(exact(ima), internal::cast_to<V>{});
  }

  template <class V, class I>
  casted_image<I&, V> imcast(Image<I>& ima)
  {
    return casted_image<I&, V>(exact(ima), internal::cast_to<V>{});
  }

  template <class V, class I>
  casted_image<I, V> imcast(Image<I>&& ima)
  {
    return casted_image<I, V>(move_exact(ima), internal::cast_to<V>{});
  }

  namespace experimental
  {
    template <class V, class IntputImage>
    casted_image<IntputImage, V> imcast(IntputImage ima)
    {
      return view::transform(std::move(ima), internal::cast_to<V>{});
    }
  } // namespace experimental
} // namespace mln