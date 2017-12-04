#ifndef MLN_MORPHO_MAXTREE_NODE_HPP
#define MLN_MORPHO_MAXTREE_NODE_HPP

namespace mln
{

  namespace morpho
  {
    struct maxtree_node
    {
      typedef unsigned size_type;
      static const unsigned Nil = -1;

      size_type m_parent;
      size_type m_prev;
      size_type m_next;
      double m_pos;
    };
  }
}

#endif // ! MLN_MORPHO_MAXTREE_NODE_HPP
