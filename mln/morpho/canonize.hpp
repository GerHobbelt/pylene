#ifndef CANONIZE_HPP
# define CANONIZE_HPP

namespace mln
{
  namespace morpho
  {

    namespace internal
    {
      template <typename V>
      struct Canonizer
      {
	Canonizer(const image2d<V>& ima, image2d<std::size_t>& parent, std::size_t* S)
	  : m_ima(ima), m_parent(parent), m_out (S)
	{
	  resize(m_dejavu, m_ima, m_ima.border(), false);
	}


	void canonize(std::size_t p)
	{
	  if (!m_dejavu[p])
	    {
	      std::size_t q = m_parent[p];
	      if (q != p) {
		canonize(q);
		if (m_ima[q] == m_ima[m_parent[q]])
		  m_parent[p] = m_parent[q];
	      }
	      *(m_out++) = p;
	      m_dejavu[p] = true;
	    }
	}


	const image2d<V>&	m_ima;
	image2d<std::size_t>&	m_parent;
	std::size_t*		m_out;
	image2d<bool>		m_dejavu;
      };

    }

    template <typename V>
    void
    canonize(const image2d<V>& ima,
	     image2d<std::size_t>& parent,
	     std::size_t*	   S)
    {
      internal::Canonizer<V> o(ima, parent, S);

      mln_pixter(p, ima);
      mln_forall(p)
	o.canonize(p->index());
    }


    template <typename V>
    void
    canonize(const image2d<V>& ima,
	     const std::vector<std::size_t>& S,
	     image2d<std::size_t>& parent)
    {
      for(std::size_t p: S)
	{
	  std::size_t q = parent[p];
	  if (ima[q] == ima[parent[q]])
	    parent[p] = parent[q];
	}
    }

  }
}

#endif // ! CANONIZE_HPP
