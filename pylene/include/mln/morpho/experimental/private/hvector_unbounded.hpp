#pragma once

#include <mln/core/assert.hpp>
#include <mln/core/concept/new/images.hpp>
#include <mln/data/experimental/histogram.hpp>

#include <algorithm>
#include <numeric>


namespace  mln::morpho::experimental::details
{


  /// \brief Set of points ordered hierarchically into a list of points (vector implemented)
  ///
  /// \tparam P                 Type of point
  template <class P>
  class hvectors_unbounded;


  /// \brief Base implementation
  template <>
  class hvectors_unbounded<void>
  {
  public:
    bool empty(int level) const noexcept;

    /// Get the level 𝑙 of the lowest non-empty queue with level ≤ 𝑙
    /// such that every queue at level x with level ≤ x < 𝑙 are empty.
    int lower_bound(int level) const noexcept;

    /// Get the level 𝑙 of the highest non-empty queue with 𝑙 < level
    /// such that every queue at level x with  𝑙 < x ≤ level are empty.
    int upper_bound(int level) const noexcept;



  protected:
    /// \param nlevel Number of levels
    /// \param size Size in bytes of an element
    hvectors_unbounded(int nlevel, std::size_t size);

    /// Realloc the buffer at a given level to double the capacity
    /// \param level The level to free
    /// \param size Size in bytes of an element
    void resize(int level, std::size_t size);

    /// Free the memory
    void release();

    virtual ~hvectors_unbounded() = default;
    virtual void uninitialized_copy_n(void* in, std::size_t count, void* out) = 0;
    virtual void destroy_n(void* buffer, std::size_t count) = 0;


    struct node_t
    {
      void* begin;
      int   size;
      int   capacity;
    };

    node_t*     m_lists; /* List of unitialized storage */
    int         m_nlevels;
  };


  template <class P>
  class hvectors_unbounded : public hvectors_unbounded<void>
  {
  public:
    hvectors_unbounded(int nlevel);
    ~hvectors_unbounded() final;

    hvectors_unbounded(const hvectors_unbounded&) = delete;
    hvectors_unbounded(hvectors_unbounded&&) = delete;
    hvectors_unbounded& operator= (const hvectors_unbounded&) = delete;
    hvectors_unbounded& operator= (hvectors_unbounded&&) = delete;


    void                push_front(int level, P p) noexcept;
    P                   pop_front(int level) noexcept;

    P                   back(int level) const noexcept;
    P                   front(int level) const noexcept;

  private:
    void uninitialized_copy_n(void* in, std::size_t count, void* out) final;
    void destroy_n(void* buffer, std::size_t count) final;



    using base = hvectors_unbounded<void>;
  };




  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  inline int hvectors_unbounded<void>::lower_bound(int level) const noexcept
  {
    mln_precondition(level < m_nlevels);

    while (level < m_nlevels && m_lists[level].size == 0)
      ++level;
    return level;
  }

  inline int hvectors_unbounded<void>::upper_bound(int level) const noexcept
  {
    mln_precondition(level < m_nlevels);

    while (level >= 0 && m_lists[level].size == 0)
      --level;
    return level;
  }

  inline bool hvectors_unbounded<void>::empty(int level) const noexcept
  {
    return m_lists[level].size == 0;
  }


  template <class P>
  inline hvectors_unbounded<P>::hvectors_unbounded(int nlevel)
    : base{nlevel, sizeof(P)}
  {
  }

  template <class P>
  inline hvectors_unbounded<P>::~hvectors_unbounded()
  {
    this->release();
  }

  template <class P>
  void hvectors_unbounded<P>::uninitialized_copy_n(void* in, std::size_t count, void* out)
  {
    std::uninitialized_copy_n((P*)in, count, (P*)out);
  }

  template <class P>
  void hvectors_unbounded<P>::destroy_n(void* in, std::size_t count)
  {
    std::destroy_n((P*)in, count);
  }




  template <class P>
  inline void hvectors_unbounded<P>::push_front(int level, P p) noexcept
  {
    if (m_lists[level].size == m_lists[level].capacity)
      this->resize(level, sizeof(P));

    P*  buffer = reinterpret_cast<P*>(m_lists[level].begin);
    P*  e      = buffer + m_lists[level].size++;
    new ((void*)e) P(p);
  }

  template <class P>
  inline P hvectors_unbounded<P>::pop_front(int level) noexcept
  {
    mln_precondition(!this->empty(level) && "Empty list");


    P* buffer = reinterpret_cast<P*>(m_lists[level].begin);

    --m_lists[level].size;
    P tmp = std::move(buffer[m_lists[level].size]);
    std::destroy_at(buffer + m_lists[level].size);
    return tmp;
  }



  template <class P>
  inline P hvectors_unbounded<P>::front(int level) const noexcept
  {
    mln_precondition(!this->empty(level) && "Empty list");

    P* buffer = reinterpret_cast<P*>(m_lists[level].begin);
    return buffer[m_lists[level].size - 1];
  }


} // namespace mln::morpho::experimental::detail
