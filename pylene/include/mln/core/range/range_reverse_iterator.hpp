#pragma once

namespace mln
{

  template <typename ReversibleRange>
  struct range_reverse_iterator
  {
    typedef typename ReversibleRange::reverse_iterator type;
  };

} // namespace mln
