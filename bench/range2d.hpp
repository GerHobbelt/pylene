#pragma once
#include <range/v3/view_facade.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/span.hpp>
#include <range/v3/view/zip.hpp>
#include <boost/preprocessor/punctuation/remove_parens.hpp>

#include <range/v3/view/single.hpp>
#include <type_traits>

#include <mln/core/foreach.hpp>

#define mln_foreach2(PROTECTED_DECL, RNG)                               \
  for (auto&& __mln_inner_rng : RNG)                                    \
    for (BOOST_PP_REMOVE_PARENS(PROTECTED_DECL) : forward_to_zip(__mln_inner_rng))


// template<class Tuple, std::size_t... I>
// auto forward_to_zip_impl(Tuple&& rngs, std::index_sequence<I...>)
// {
//   return ranges::view::zip(std::get<I>(std::forward<Tuple>(rngs))...);
// }


template<class Rng1, class Rng2>
auto forward_to_zip(ranges::common_pair<Rng1, Rng2> rngs)
{
  return ranges::view::zip(rngs.first, rngs.second);
}

template<class Other>
decltype(auto) forward_to_zip(Other&& rng)
{
  return std::forward<Other>(rng);
}

template <class T>
struct image2d_view
{
  image2d_view() = default;
  image2d_view(mln::image2d<T>& ima)
  {
    buffer = &(ima.at(0,0));
    stride = ima.index_strides()[0];
    width = ima.ncols();
    height = ima.nrows();
  }

  image2d_view(const mln::image2d<std::remove_const_t<T>>& ima)
  {
    buffer = &(ima.at(0,0));
    stride = ima.index_strides()[0];
    width = ima.ncols();
    height = ima.nrows();
  }

  std::ptrdiff_t delta_index(mln::point2d p) const
  {
    return stride * p[0] + p[1];
  }

  T* buffer;
  std::ptrdiff_t stride;
  int width;
  int height;
};



template <class T>
class value_range2d_outer : public ranges::view_facade<value_range2d_outer<T>, ranges::finite>
{
  friend ranges::range_access;

  struct cursor
  {
    auto read() const { return ranges::make_span(m_lineptr, m_width); }
    bool equal(const cursor& other) const { return m_lineptr == other.m_lineptr; }
    void next() { m_lineptr += m_stride; }

    cursor() = default;
    cursor(T* lineptr, std::ptrdiff_t stride, int width)
      : m_lineptr(lineptr), m_stride(stride), m_width(width)
      {
      }

  private:
    T*                  m_lineptr;
    std::ptrdiff_t      m_stride;
    int                 m_width;
  };

  cursor begin_cursor() const { return cursor(m_ima.buffer, m_ima.stride, m_ima.width);  }
  cursor end_cursor() const { return cursor(m_ima.buffer + m_ima.height * m_ima.stride, m_ima.stride, m_ima.width);  }

public:
  value_range2d_outer() = default;
  value_range2d_outer(image2d_view<T> f)
    : m_ima(std::move(f))
    {
    }

private:
  image2d_view<T> m_ima;
};




template <class T>
class pixel_range2d_inner : public ranges::view_facade<pixel_range2d_inner<T>, ranges::finite>
{
  friend ranges::range_access;

public:
  class cursor_t
  {
    friend ranges::range_access;
  public:
    cursor_t() = default;
    cursor_t(const image2d_view<T>* image, T* lineptr, int y, int x) : m_image(image), m_lineptr(lineptr), m_y(y), m_x(x) {}

    // Pixel interface
    T& val() const { return m_lineptr[m_x]; }
    mln::point2d point() const { return {m_y, m_x}; }
    const image2d_view<T>& image() const { return *m_image; }

  private:
    const cursor_t& read() const { return *this; }
    bool  equal(const cursor_t& other) const { return m_x == other.m_x; }
    void  next() { m_x++; }

  public:
    void  advance(int dx, int dy)
      {
        m_lineptr += m_image->stride * dy;
        m_y += dy;
        m_x += dx;
      }

  private:
    const image2d_view<T>* m_image;
    T*                     m_lineptr;
    int              m_y;
    std::ptrdiff_t   m_x; // Note: using int prevent vectorization
  };

  //using cursor = range2d_cursor<T, E>;

private:
  cursor_t begin_cursor() const { return cursor_t(&m_image, m_lineptr, m_y, 0);  }
  cursor_t end_cursor() const { return cursor_t(&m_image, m_lineptr + m_image.width, m_y, m_image.width); }

public:
  pixel_range2d_inner() = default;
  pixel_range2d_inner(const image2d_view<T>& ima, T* lineptr, int y)
    : m_image(ima), m_lineptr(lineptr),  m_y(y)
    {
    }

private:
  image2d_view<T>  m_image;
  T*               m_lineptr;
  int              m_y;
};



template <class T>
class pixel_range2d_outer : public ranges::view_facade<pixel_range2d_outer<T>, ranges::finite>
{
  friend ranges::range_access;

  struct cursor
  {
    auto read() const { return pixel_range2d_inner<T>(m_image, m_image.buffer + m_y * m_image.stride, m_y); }
    bool equal(const cursor& other) const { return m_y == other.m_y; }
    void next() { m_y++; }

    cursor() = default;
    cursor(image2d_view<T> ima, int y)
      : m_image(ima), m_y(y)
      {
      }

  private:
    image2d_view<T>   m_image;
    int               m_y;
  };

  cursor begin_cursor() const { return cursor(m_ima, 0);  }
  cursor end_cursor() const { return cursor(m_ima, m_ima.height);  }

public:
  pixel_range2d_outer() = default;
  pixel_range2d_outer(const image2d_view<T>& ima)
    : m_ima(ima)
    {
    }

private:
  image2d_view<T> m_ima;
};

template <class T>
struct pixel_range2d : public segmented_range_base
{
public:
  pixel_range2d() = default;
  pixel_range2d(image2d_view<T> f)
    : m_ima(std::move(f))
  {}

  class linear_iterator : public std::iterator<std::input_iterator_tag, T>
  {
    image2d_view<T> m_ima;
    std::ptrdiff_t index_x;
    std::ptrdiff_t index_y;
  public:
    linear_iterator(image2d_view<T> view)
      : m_ima(view)
      , index_x(0)
      , index_y(0)
    {}

    linear_iterator(image2d_view<T> view, std::ptrdiff_t i_x, std::ptrdiff_t i_y)
      : m_ima(view)
      , index_x(i_x)
      , index_y(i_y)
    {}

    image2d_view<T> getView() const { return m_ima; }
    std::ptrdiff_t getIndexX() const { return index_x; }
    std::ptrdiff_t getIndexY() const { return index_y; }

    linear_iterator& operator++()
    {
      index_x++;
      if (index_x == m_ima.width)
      {
        index_x = 0;
        index_y++;
      }
      return *this;
    }

    bool operator==(const linear_iterator& other) const
    {
      return m_ima.buffer == other.getView().buffer
              && index_x == other.getIndexX()
              && index_y == other.getIndexY();
    }

    bool operator!=(const linear_iterator& other) const {return !operator==(other); }

    class pixel_interface
    {
      image2d_view<T> m_ima;
      std::ptrdiff_t m_x;
      std::ptrdiff_t m_y;
    public:
      pixel_interface(image2d_view<T> view, std::ptrdiff_t i_x, std::ptrdiff_t i_y)
        : m_ima(view)
        , m_x(i_x)
        , m_y(i_y)
      {}

      T& val() const { return m_ima.buffer[m_x + m_y * m_ima.stride]; }
      mln::point2d point() const { return {m_y, m_x}; }
      const image2d_view<T>& image() const { return m_ima; }
    };

    pixel_interface operator*() {return pixel_interface(m_ima, index_x, index_y); }
  };

  /* Les lignes commentées ci-dessous sont les bonnes version des méthodes begin et end
  mais elles empêchent la classe de correspondre au concept InputRange, les autres méthodes
  renvoyant un pointeur ne posent pas se problème */

  //linear_iterator begin() const { return linear_iterator(m_ima); }
  //linear_iterator end() const { return linear_iterator(m_ima, 0, m_ima.height); }

  T* begin() { return nullptr; }
  T* end() { return nullptr; }

  /*********************************************/

  pixel_range2d_outer<T> outer() const { return pixel_range2d_outer<T>(m_ima); }

private:
  image2d_view<T> m_ima;
};

template <class T>
struct value_range2d : public segmented_range_base, ranges::view_facade<value_range2d<T>, ranges::finite>
{
public:
  value_range2d() = default;
  value_range2d(image2d_view<T> f)
    : m_ima(std::move(f))
  {}

  class linear_iterator : public std::iterator<std::input_iterator_tag, T>
  {
    image2d_view<T> m_ima;
    std::ptrdiff_t index_x;
    std::ptrdiff_t index_y;
  public:
    linear_iterator(image2d_view<T> view)
      : m_ima(view)
      , index_x(0)
      , index_y(0)
    {}

    linear_iterator(image2d_view<T> view, std::ptrdiff_t i_x, std::ptrdiff_t i_y)
      : m_ima(view)
      , index_x(i_x)
      , index_y(i_y)
    {}

    image2d_view<T> getView() const { return m_ima; }
    std::ptrdiff_t getIndexX() const { return index_x; }
    std::ptrdiff_t getIndexY() const { return index_y; }

    linear_iterator& operator++()
    {
      index_x++;
      if (index_x == m_ima.width)
      {
        index_x = 0;
        index_y++;
      }
      return *this;
    }

    bool operator==(const linear_iterator& other) const
    {
      return m_ima.buffer == other.getView().buffer
              && index_x == other.getIndexX()
              && index_y == other.getIndexY();
    }

    bool operator!=(const linear_iterator& other) const {return !operator==(other); }
    T& operator*() {return *(m_ima.buffer + index_x + index_y * m_ima.stride); }
  };

  /* Les lignes commentées ci-dessous sont les bonnes version des méthodes begin et end
  mais elles empêchent la classe de correspondre au concept InputRange, les autres méthodes
  renvoyant un pointeur ne posent pas se problème */

  //linear_iterator begin() const { return linear_iterator(m_ima); }
  //linear_iterator end() const { return linear_iterator(m_ima, 0, m_ima.height); }

  T* begin() { return nullptr; }
  T* end() { return nullptr; }

  /*****************************************************************/

  value_range2d_outer<T> outer() const { return value_range2d_outer<T>(m_ima); }

private:
  image2d_view<T> m_ima;
};

template <class Rng, typename = std::enable_if_t<!IsSegmentedRange<std::remove_reference_t<Rng>>::value>>
auto Rng_Specify(Rng&& rng)
{
  return ranges::single_view(std::forward<Rng>(rng));
}

template <class Rng, typename = std::enable_if_t<IsSegmentedRange<std::decay_t<Rng>>::value>>
decltype(auto) Rng_Specify(Rng&& rng)
{
  return rng.outer();
}

template <class T>
value_range2d<const T> values_of(const mln::image2d<T>& ima)
{
  image2d_view<const T> view(ima);
  return value_range2d<const T>(view);
}

template <class T>
value_range2d<T> values_of(mln::image2d<T>& ima)
{
  image2d_view<T> view(ima);
  return value_range2d<T>(view);
}

template <class T>
pixel_range2d<const T> pixels_of(const mln::image2d<T>& ima)
{
  image2d_view<const T> view(ima);
  return pixel_range2d<const T>(view);
}

template <class T>
pixel_range2d<T> pixels_of(mln::image2d<T>& ima)
{
  image2d_view<T> view(ima);
  return pixel_range2d<T>(view);
}

#define mln_foreach_new(PROTECTED_DECL, RNG)                                                                           \
  MLN_DECL_VAR(__mln_has_been_broken, false)                                                                           \
  for (auto&& __mln_inner_rng : Rng_Specify(RNG))                                                                      \
    if (__mln_has_been_broken.get())                                                                                   \
      break;                                                                                                           \
    else                                                                                                               \
      for (BOOST_PP_REMOVE_PARENS(PROTECTED_DECL) : __mln_inner_rng)                                                   \
        if (__mln_has_been_broken.get())                                                                               \
          break;                                                                                                       \
        else                                                                                                           \
            for (__mln_has_been_broken.set(true) ; __mln_has_been_broken.get(); __mln_has_been_broken.set(false))
