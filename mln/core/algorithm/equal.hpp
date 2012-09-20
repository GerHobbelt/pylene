#ifndef MLN_CORE_ALGORITHM_EQUAL_HPP
# define MLN_CORE_ALGORITHM_EQUAL_HPP

# include <mln/core/concept/image.hpp>
# include <mln/core/image/zip_image.hpp>

namespace mln
{

  /// \brief Compare if two image are equals.
  ///
  /// Two image are said equal if their domains
  /// are equals and have the same values.
  ///
  /// \param ima1 First image
  /// \param ima2 Second image
  ///
  /// \tparam I must model a Readable Forward Image
  /// \tparam J must model a Readable Forward Image
  ///
  /// \return True if image are equals.
  ///
  template <typename I, typename J>
  bool
  equal(const Image<I>& ima1, const Image<J>& ima2);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename I, typename J>
  inline
  bool
  equal(const Image<I>& ima1, const Image<J>& ima2)
  {
    // FIXME: add test for domain comparison

    for (auto x: imzip(exact(ima1), exact(ima2)).values())
      if (boost::get<0>(x) != boost::get<1>(x))
        return false;

    return true;
  };


} // end of namespace mln

#endif //!MLN_CORE_ALGORITHM_EQUAL_HPP
