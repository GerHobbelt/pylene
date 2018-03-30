///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_GENERATOR_HPP_INCLUDED
#define CPPCORO_GENERATOR_HPP_INCLUDED

#include <experimental/coroutine>
#include <type_traits>
#include <utility>
#include <iterator>

namespace cppcoro
{
  // template <typename T>
  // class generator;

  namespace detail
  {
    template <class T>
    class generator_iterator;

    template <typename T>
    class generator_promise
    {
    public:
      using value_type = std::remove_reference_t<T>;
      using reference_type = std::conditional_t<std::is_reference_v<T>, T, const T&>;
      using pointer_type = value_type*;

      generator_promise() = default;

      generator_iterator<T> get_return_object() noexcept
      {
          using coroutine_handle = std::experimental::coroutine_handle<generator_promise<T>>;
          return generator_iterator<T>{coroutine_handle::from_promise(*this)};
      }

      constexpr std::experimental::suspend_always initial_suspend() const { return {}; }
      constexpr std::experimental::suspend_always final_suspend() const { return {}; }


      std::experimental::suspend_always yield_value(T value) noexcept
      {
        m_value = value;
        return {};
      }

      void unhandled_exception() noexcept { /* std::rethrow_exception(std::current_exception()); */ }

      void return_void() {}

      T value() const noexcept { return m_value; }

      // Don't allow any use of 'co_await' inside the generator coroutine.
      template <typename U>
      std::experimental::suspend_never await_transform(U&& value) = delete;

    private:
      T m_value;
    };


    template <typename T>
    class generator_iterator
    {
    public:
      using promise_type = generator_promise<T>;

    private:
      using coroutine_handle = std::experimental::coroutine_handle<promise_type>;

    public:
      struct sentinel_t {};
      using iterator_category = std::input_iterator_tag;
      // What type should we use for counting elements of a potentially infinite sequence?
      using difference_type = std::size_t;
      using value_type = std::remove_reference_t<T>;
      using reference = const value_type&;
      using pointer = const value_type*;

      explicit generator_iterator(std::nullptr_t) noexcept : m_coroutine(nullptr) {}

      explicit generator_iterator(coroutine_handle coroutine) noexcept : m_coroutine(coroutine) {}

      ~generator_iterator() { if (m_coroutine) {} }

      bool operator==(const sentinel_t&) const noexcept { return m_coroutine.done(); }

      bool operator!=(const sentinel_t&) const noexcept { return !m_coroutine.done(); }

      generator_iterator& operator++()
      {
        m_coroutine.resume();
        return *this;
      }

      // Don't support post-increment as that would require taking a
      // copy of the old value into the returned iterator as there
      // are no guarantees it's still going to be valid after the
      // increment is executed.
      generator_iterator operator++(int) = delete;

      T operator*() const noexcept { return m_coroutine.promise().value(); }

      pointer operator->() const noexcept { return std::addressof(m_coroutine.promise().value()); }

    public:
      coroutine_handle m_coroutine;
    };

    template <class T>
    auto& begin(generator_iterator<T>& x)
    {
      x.m_coroutine.resume();
      return x;
    }

    template <class T>
    constexpr
    typename generator_iterator<T>::sentinel_t end(generator_iterator<T>&)
    {
      return {};
    }
  }

  template <class T>
  using generator = detail::generator_iterator<T>;

}


  /*

  template <typename T>
  class generator
  {
  public:
    using promise_type = detail::generator_promise<T>;
    using iterator = detail::generator_iterator<T>;

    generator() noexcept : m_coroutine(nullptr) {}

    generator(generator&& other) noexcept : m_coroutine(other.m_coroutine) { other.m_coroutine = nullptr; }

    generator(const generator& other) = delete;

    ~generator()
    {
      if (m_coroutine)
      {
        m_coroutine.destroy();
      }
    }

    generator& operator=(generator other) noexcept
    {
      swap(other);
      return *this;
    }

    iterator begin()
    {
      if (m_coroutine)
      {
        m_coroutine.resume();
        return iterator{m_coroutine};
      }

      return iterator{nullptr};
    }

    constexpr
    typename iterator::sentinel_t end() const { return {}; }

    void swap(generator& other) noexcept { std::swap(m_coroutine, other.m_coroutine); }

  private:
    friend class detail::generator_promise<T>;

    explicit generator(std::experimental::coroutine_handle<promise_type> coroutine) noexcept : m_coroutine(coroutine) {}

    std::experimental::coroutine_handle<promise_type> m_coroutine;
  };

  template <typename T>
  void swap(generator<T>& a, generator<T>& b)
  {
    a.swap(b);
  }

  namespace detail
  {
    template <typename T>
    generator<T> generator_promise<T>::get_return_object() noexcept
    {
      using coroutine_handle = std::experimental::coroutine_handle<generator_promise<T>>;
      return generator<T>{coroutine_handle::from_promise(*this)};
    }
  }

  template <typename FUNC, typename T>
  generator<std::result_of_t<FUNC && (T&)>> fmap(FUNC func, generator<T> source)
  {
    for (auto& value : source)
    {
      co_yield std::invoke(func, value);
    }
  }
}

  */

#endif
