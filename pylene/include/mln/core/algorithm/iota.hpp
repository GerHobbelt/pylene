#pragma once
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

namespace mln
{

  /// \brief \p iota traverses the image, the `i`th pixel is assigned
  /// with `val + i`
  ///
  /// \ingroup Algorithms
  ///
  /// \param[out] ima The output image.
  /// \param val The value to assign.
  ///
  /// \return The image.
  ///
  /// \tparam I is a model of the Writable Forward Image.
  /// \tparam Value must be convertible to Image's value type.
  template <typename I, typename Value>
  [[deprecated]] I& iota(Image<I>& output, Value val);

  /// \overload
  /// \ingroup Algorithms
  template <typename I, typename Value>
  [[deprecated]] I&& iota(Image<I>&& output, Value val);


  namespace experimental
  {
    template <typename OutputImage, typename Value>
    void iota(OutputImage output, Value val);
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {

    template <typename I, typename V>
    void iota(I& ima, V v)
    {
      mln_viter(vout, ima);
      mln_forall (vout)
        *vout = v++;
    }
  }

  template <typename I, typename Value>
  inline I&& iota(Image<I>&& output_, Value val)
  {
    iota(output_, val);
    return move_exact(output_);
  }

  template <typename I, typename Value>
  inline I& iota(Image<I>& output_, Value val)
  {
    I& output = exact(output_);
    impl::iota(output, val);
    return output;
  }



  namespace experimental
  {
    template <typename OutputImage, typename Value>
    void iota(OutputImage output, Value val)
    {
      static_assert(mln::is_a<OutputImage, Image>());
      static_assert(std::is_convertible_v<Value, image_value_t<OutputImage>>);

      for (auto row : mln::ranges::rows(output.new_values()))
        for (auto& v : row)
          v = val++;
    }
  } // namespace experimental

} // end of namespace mln


