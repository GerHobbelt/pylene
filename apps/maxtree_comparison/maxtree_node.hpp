#pragma once

namespace mln
{
  namespace morpho
  {
    struct maxtree_node
    {
      typedef unsigned      size_type;
      static const unsigned Nil = -1;

      size_type m_parent;
      size_type m_prev;
      size_type m_next;
      double    m_pos;
    };
  } // namespace morpho
} // namespace mln
