#ifndef MLN_CORE_ALGORITHM_IOTA_HPP
# define MLN_CORE_ALGORITHM_IOTA_HPP

# include <mln/core/concept/image.hpp>
# include <boost/range/algorithm_ext/iota.hpp>



namespace mln {

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
  I&
  iota(Image<I>& output, Value val);


  /// \overload
  /// \ingroup Algorithms
  template <typename I, typename Value>
  I&&
  iota(Image<I>&& output, Value val);


/******************************************/
/****          Implementation          ****/
/******************************************/


  template <typename I, typename Value>
  inline
  I&&
  iota(Image<I>&& output_, Value val)
  {
    I&& output = fwd_exact(output_);
    boost::iota(output.values(), val);
    return std::forward<I>(output);
  }

  template <typename I, typename Value>
  inline
  I&
  iota(Image<I>& output_, Value val)
  {
    I& output = exact(output_);
    boost::iota(output.values(), val);
    return output;
  }

} // end of namespace mln

#endif //!MLN_CORE_ALGORITHM_IMFILL_HPP
