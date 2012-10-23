#ifndef MLN_MORPHO_BOUNDED_HQUEUE_HPP
# define MLN_MORPHO_BOUNDED_HQUEUE_HPP

# include <memory>
# include <numeric>
# include <array>

namespace mln
{

  template <typename T, unsigned nLevel, typename Allocator = std::allocator<T> >
  struct bounded_hqueue
  {
    bounded_hqueue();
    explicit bounded_hqueue(const size_t* histo);
    ~bounded_hqueue();

    void init(const size_t* histo);

    bool empty(unsigned l) const;
    void push_at_level(const T& x, unsigned l);
    T	 pop_at_level(unsigned l);

  private:
    std::array<T*, nLevel> m_head;
    std::array<T*, nLevel> m_tail;
    Allocator   m_allocator;
    T*		m_q;
    T*		m_end;
  };

  /********************/
  /** Implementation **/
  /********************/

  template <typename T, unsigned nLevel, typename Allocator >
  inline
  bounded_hqueue<T, nLevel, Allocator>::bounded_hqueue()
    : m_head {{NULL,}}, m_tail {{NULL,}}, m_q(NULL), m_end (NULL)
  {
  }

  template <typename T, unsigned nLevel, typename Allocator>
  inline
  bounded_hqueue<T, nLevel, Allocator>::bounded_hqueue(const size_t* histo)
  {
    init(histo);
  }

  template <typename T, unsigned nLevel, typename Allocator>
  inline
  bounded_hqueue<T, nLevel, Allocator>::~bounded_hqueue()
  {
    if (m_q != NULL)
      {
	for (unsigned i = 0; i < nLevel; ++i)
	  for (T* ptr = m_head[i]; ptr != m_tail[i]; ++ptr)
	    m_allocator.destroy(ptr);
	m_allocator.deallocate(m_q, m_end - m_q);
      }
  }


  template <typename T, unsigned nLevel, typename Allocator>
  inline
  void
  bounded_hqueue<T, nLevel, Allocator>::init(const size_t* histo)
  {
    mln_precondition(m_q == NULL);

    unsigned nelements = std::accumulate(histo, histo + nLevel, 0u);
    m_q = m_allocator.allocate(nelements);
    m_end = m_q + nelements;
    unsigned n = 0;
    for (unsigned i = 0; i < nLevel; ++i)
      {
	m_head[i] = m_tail[i] = m_q + n;
	n += histo[i];
      }
  }


  template <typename T, unsigned nLevel, typename Allocator>
  inline
  bool
  bounded_hqueue<T, nLevel, Allocator>::empty(unsigned level) const
  {
    mln_precondition(level < nLevel);
    return m_head[level] == m_tail[level];
  }

  template <typename T, unsigned nLevel, typename Allocator>
  inline
  void
  bounded_hqueue<T, nLevel, Allocator>::push_at_level(const T& x, unsigned level)
  {
    mln_precondition(level < nLevel);
    mln_precondition(m_tail[level] < m_end and (level == nLevel-1 or m_tail[level] < m_head[level+1]));
    m_allocator.construct(m_tail[level]++, x);
  }

  template <typename T, unsigned nLevel, typename Allocator>
  inline
  T
  bounded_hqueue<T, nLevel, Allocator>::pop_at_level(unsigned level)
  {
    mln_precondition(level < nLevel);
    mln_precondition(!empty(level));
    T x = std::move(*(--m_tail[level]));
    m_allocator.destroy(m_tail[level]);
    return x;
  }

}

#endif // !MLN_MORPHO_BOUNDED_HQUEUE_HPP
