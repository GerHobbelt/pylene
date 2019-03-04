#pragma once

#include <mln/core/range/category.hpp>

#include <boost/range.hpp>


namespace mln
{

  /// Meta-functions
  /// \{
  using boost::range_iterator;
  using boost::range_pointer;
  using boost::range_reference;
  using boost::range_value;
  // using boost::range_category; Replaced by mln::range_category
  using boost::has_range_const_iterator;
  using boost::has_range_iterator;
  using boost::range_difference;
  using boost::range_reverse_iterator;

} // namespace mln
