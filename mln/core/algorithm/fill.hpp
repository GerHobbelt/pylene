#ifndef MLN_CORE_ALGORITHM_IMFILL_HPP
# define MLN_CORE_ALGORITHM_IMFILL_HPP

# include <mln/core/concept/image.hpp>
# include <boost/range/algorithm/fill.hpp>


namespace mln {

  /// \brief \p fill assigns the value \p val to every element of the image \p
  /// ima.
  /// \ingroup Algorithms
  ///
  /// \param[out] output The output image.
  /// \param val The value to assign.
  ///
  /// \return The image.
  ///
  /// \tparam OutputImage is a model of the Writable Forward Image.
  /// \tparam Value must be convertible to Image's value type.
  /// \ingroup algorithms
  template <typename OutputImage, typename Value>
  OutputImage&
  fill(Image<OutputImage>& output, const Value& val);


  /// \overload
  /// \ingroup Algorithms
  template <typename OutputImage, typename Value>
  OutputImage&&
  fill(Image<OutputImage>&& output, const Value& val);

/******************************************/
/****          Implementation          ****/
/******************************************/


  template <typename OutputImage, typename Value>
  OutputImage&&
  fill(Image<OutputImage>&& output_, const Value& val)
  {
    OutputImage&& output = fwd_exact(output_);
    boost::fill(output.values(), val);
    return fwd_exact(output);
  }


  template <typename OutputImage, typename Value>
  OutputImage&
  fill(Image<OutputImage>& output_, const Value& val)
  {
    OutputImage& output = exact(output_);
    boost::fill(output.values(), val);
    return output;
  }


} // end of namespace mln

#endif //!MLN_CORE_ALGORITHM_IMFILL_HPP
