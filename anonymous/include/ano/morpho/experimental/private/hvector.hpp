#pragma once

#include <ano/core/assert.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/data/experimental/histogram.hpp>

#include <algorithm>
#include <numeric>


namespace ano::morpho::experimental::details
{


  /// \brief Set of points ordered hierarchically into a list of points (vector implemented)
  ///
  /// \tparam P                 Type of point
  template <class P>
  class hvectors;


  /// \brief Base implementation
  template <>
  class hvectors<void>
  {
  public:
    bool empty(int level) const noexcept;

  protected:
    /// \param count Number of elements in the histogram
    /// \param size Size in bytes of an element
    hvectors() = default;
    ~hvectors();

    void init(const std::size_t* cumulated_histogram, std::size_t count, std::size_t size);


    struct vector_t
    {
      void* begin;
      void* end;
    };
    vector_t* m_lists; /* List of unitialized storage */
  };


  template <class P>
  class hvectors : public hvectors<void>
  {
  public:
    template <class J>
    hvectors(J&& f);
    ~hvectors() = default;

    hvectors(const hvectors&)            = delete;
    hvectors(hvectors&&)                 = delete;
    hvectors& operator=(const hvectors&) = delete;
    hvectors& operator=(hvectors&&)      = delete;


    void push_back(int level, P p) noexcept;
    P    pop_back(int level) noexcept;
    P    pop_front(int level) noexcept;

    P back(int level) const noexcept;
    P front(int level) const noexcept;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class P>
  template <class J>
  inline hvectors<P>::hvectors(J&& f)
  {
    auto hist = ano::data::experimental::histogram(std::forward<J>(f));
    std::partial_sum(hist.begin(), hist.end(), hist.begin());
    this->init(hist.data(), hist.size(), sizeof(P));
  }


  inline bool hvectors<void>::empty(int level) const noexcept
  {
    return m_lists[level].begin == m_lists[level].end;
  }


  template <class P>
  inline void hvectors<P>::push_back(int level, P p) noexcept
  {
    P*& end = reinterpret_cast<P*&>(m_lists[level].end);
    new ((void*)end++) P(p);
  }


  template <class P>
  inline P hvectors<P>::pop_front(int level) noexcept
  {
    ano_precondition(!this->empty(level) && "Empty list");

    P*& begin = reinterpret_cast<P*&>(m_lists[level].begin);
    P   tmp   = std::move(*begin);
    std::destroy_at(begin++);
    return tmp;
  }

  template <class P>
  inline P hvectors<P>::pop_back(int level) noexcept
  {
    ano_precondition(!this->empty(level) && "Empty list");

    P*& end = reinterpret_cast<P*&>(m_lists[level].end);
    --end;
    P tmp = std::move(*end);
    std::destroy_at(end);
    return tmp;
  }


  template <class P>
  inline P hvectors<P>::front(int level) const noexcept
  {
    ano_precondition(!this->empty(level) && "Empty list");
    return *reinterpret_cast<P*>(m_lists[level].begin);
  }

  template <class P>
  inline P hvectors<P>::back(int level) const noexcept
  {
    ano_precondition(!this->empty(level) && "Empty list");
    return reinterpret_cast<P*>(m_lists[level].end)[-1];
  }


} // namespace ano::morpho::experimental::details
