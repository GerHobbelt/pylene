#pragma once

#include <ano/core/value/value_traits.hpp>

#include <functional>


namespace ano
{

  template <typename T, typename StrictWeakOrdering>
  struct Index;

  template <typename T>
  struct Index<T, std::less<T>>
  {
    Index() = default;
    explicit Index(T v)
      : x(v)
    {
    }

    // operator long () const { return x; }
    // operator unsigned long () const { return x; }
    operator int() const { return x; }
    // operator unsigned () const { return x; }
    // operator T () const { return x; }

    Index& operator++()
    {
      ++x;
      return *this;
    }
    Index& operator--()
    {
      --x;
      return *this;
    }
    Index operator++(int) { return Index(x++); }
    Index operator--(int) { return Index(x--); }
    bool  operator==(const Index& other) const { return x == other.x; }
    bool  operator!=(const Index& other) const { return x != other.x; }
    bool  operator<(const Index& other) const { return x < other.x; }
    bool  operator>(const Index& other) const { return x > other.x; }
    bool  operator<=(const Index& other) const { return x <= other.x; }
    bool  operator>=(const Index& other) const { return x >= other.x; }

  private:
    T x;
  };

  template <typename T>
  struct Index<T, std::greater<T>>
  {
    Index() = default;
    explicit Index(T v)
      : x(v)
    {
    }

    // operator long () const { return x; }
    // operator unsigned long () const { return x; }
    operator int() const { return x; }
    // operator unsigned () const { return x; }
    // operator T () const { return x; }

    Index& operator++()
    {
      --x;
      return *this;
    }
    Index& operator--()
    {
      ++x;
      return *this;
    }
    Index operator++(int) { return Index(x--); }
    Index operator--(int) { return Index(x++); }
    bool  operator==(const Index& other) const { return x == other.x; }
    bool  operator!=(const Index& other) const { return x != other.x; }
    bool  operator<(const Index& other) const { return x > other.x; }
    bool  operator>(const Index& other) const { return x < other.x; }
    bool  operator<=(const Index& other) const { return x >= other.x; }
    bool  operator>=(const Index& other) const { return x <= other.x; }

  private:
    T x;
  };

  template <>
  struct Index<bool, std::greater<bool>>
  {
    Index() = default;
    explicit Index(bool v)
      : x(v)
    {
    }

    // operator long () const { return x; }
    // operator unsigned long () const { return x; }
    operator int() const { return x; }
    // operator unsigned () const { return x; }
    // operator T () const { return x; }

    Index& operator++()
    {
      x = !x;
      return *this;
    }
    Index& operator--()
    {
      x = !x;
      return *this;
    }
    Index operator++(int)
    {
      x = !x;
      return Index(!x);
    }
    Index operator--(int)
    {
      x = !x;
      return Index(!x);
    }
    bool operator==(const Index& other) const { return x == other.x; }
    bool operator!=(const Index& other) const { return x != other.x; }
    bool operator<(const Index& other) const { return x > other.x; }
    bool operator>(const Index& other) const { return x < other.x; }
    bool operator<=(const Index& other) const { return x >= other.x; }
    bool operator>=(const Index& other) const { return x <= other.x; }

  private:
    bool x;
  };

  template <>
  struct Index<bool, std::less<bool>>
  {
    Index() = default;
    explicit Index(bool v)
      : x(v)
    {
    }

    // operator long () const { return x; }
    // operator unsigned long () const { return x; }
    operator int() const { return x; }
    // operator unsigned () const { return x; }
    // operator T () const { return x; }

    Index& operator++()
    {
      x = !x;
      return *this;
    }
    Index& operator--()
    {
      x = !x;
      return *this;
    }
    Index operator++(int)
    {
      x = !x;
      return Index(!x);
    }
    Index operator--(int)
    {
      x = !x;
      return Index(!x);
    }
    bool operator==(const Index& other) const { return x == other.x; }
    bool operator!=(const Index& other) const { return x != other.x; }
    bool operator<(const Index& other) const { return x < other.x; }
    bool operator>(const Index& other) const { return x > other.x; }
    bool operator<=(const Index& other) const { return x <= other.x; }
    bool operator>=(const Index& other) const { return x >= other.x; }

  private:
    bool x;
  };

  template <typename T>
  struct value_traits<Index<T, std::less<T>>, std::less<Index<T, std::less<T>>>, void>
  {
  private:
    typedef Index<T, std::less<T>> index_t;

  public:
    static constexpr unsigned quant = value_traits<T>::quant;
    static constexpr index_t  min() { return index_t(0); }
    static constexpr index_t  max() { return index_t(value_traits<T>::max()); }
    static constexpr index_t  inf() { return min(); }
    static constexpr index_t  sup() { return max(); }
  };

  template <typename T>
  struct value_traits<Index<T, std::greater<T>>, std::less<Index<T, std::greater<T>>>, void>
  {
  private:
    typedef Index<T, std::greater<T>> index_t;

  public:
    static constexpr unsigned quant = value_traits<T>::quant;
    static constexpr index_t  min() { return index_t(value_traits<T>::max()); }
    static constexpr index_t  max() { return index_t(0); }
    static constexpr index_t  inf() { return min(); }
    static constexpr index_t  sup() { return max(); }
  };

  template <typename T>
  struct value_traits<Index<T, std::less<T>>, productorder_less<Index<T, std::less<T>>>, void>
    : value_traits<Index<T, std::less<T>>, std::less<Index<T, std::less<T>>>, void>
  {
  };

  template <typename T>
  struct value_traits<Index<T, std::greater<T>>, productorder_less<Index<T, std::greater<T>>>, void>
    : value_traits<Index<T, std::greater<T>>, std::less<Index<T, std::greater<T>>>, void>
  {
  };
} // namespace ano
