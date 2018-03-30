#pragma once
#include <range/v3/view_facade.hpp>
#include <range/v3/span.hpp>
#include <range/v3/view/zip.hpp>
#include <boost/preprocessor/punctuation/remove_parens.hpp>

#define mln_foreach2(PROTECTED_DECL, RNG)                               \
  for (auto&& __mln_inner_rng : RNG)                                      \
    for (BOOST_PP_REMOVE_PARENS(PROTECTED_DECL) : forward_to_zip(__mln_inner_rng))


template<class Tuple, std::size_t... I>
auto forward_to_zip_impl(Tuple&& rngs, std::index_sequence<I...>)
{
  return ranges::view::zip(std::get<I>(std::forward<Tuple>(rngs))...);
}


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

  cursor begin_cursor() const { return cursor(m_buffer, m_stride, m_width);  }
  cursor end_cursor() const { return cursor(m_buffer + m_height * m_stride, m_stride, m_width);  }

public:
  value_range2d_outer() = default;
  value_range2d_outer(T* buffer, int width, int height, std::ptrdiff_t stride)
    : m_buffer(buffer), m_stride(stride), m_width(width), m_height(height)
    {
    }

private:
  T*                    m_buffer;
  std::ptrdiff_t        m_stride;
  int                   m_width;
  int                   m_height;
};

template <class T>
value_range2d_outer<const T> values_of(const mln::image2d<T>& ima)
{
  std::ptrdiff_t stride = ima.index_strides()[0];
  int width = ima.ncols();
  int height = ima.nrows();
  const T* buffer = &(ima.at(0,0));
  return value_range2d_outer<const T>(buffer, width, height, stride);
}

template <class T>
value_range2d_outer<T> values_of(mln::image2d<T>& ima)
{
  std::ptrdiff_t stride = ima.index_strides()[0];
  int width = ima.ncols();
  int height = ima.nrows();
  T* buffer = &(ima.at(0,0));
  return value_range2d_outer<T>(buffer, width, height, stride);
}

template <class T, class E>
class pixel_range2d_inner : public ranges::view_facade<pixel_range2d_inner<T, E>, ranges::finite>
{
  friend ranges::range_access;

  class cursor
  {
    friend ranges::range_access;
  public:
    cursor() = default;
    cursor(E* image, T* lineptr, std::ptrdiff_t lineindex, int y, int x) : m_image(image), m_lineptr(lineptr), m_lineindex(lineindex), m_y(y), m_x(x) {}

    T& val() const { return m_lineptr[m_x]; }
    mln::point2d point() const { return {m_y, m_x}; }
    E& image() const { return *m_image; }
    std::ptrdiff_t index() const { return m_lineindex + m_x; }

  private:
    const cursor& read() const { return *this; }
    bool  equal(const cursor& other) const { return m_x == other.m_x; }
    void  next() { m_x++; }

  private:
    E*             m_image;
    T*             m_lineptr;
    std::ptrdiff_t m_lineindex;
    int            m_y;
    std::ptrdiff_t m_x; // Note: using int prevent vectorization
  };

  cursor begin_cursor() const { return cursor(m_image, m_lineptr, m_lineindex, m_y, 0);  }
  cursor end_cursor() const { return cursor(nullptr, nullptr, 0, 0, m_width); }

public:
  pixel_range2d_inner() = default;
  pixel_range2d_inner(E* image, T* lineptr, std::ptrdiff_t lineindex, int width, int y)
    : m_image(image), m_lineptr(lineptr), m_lineindex(lineindex), m_width(width), m_y(y)
    {
    }

private:
  E*             m_image;
  T*             m_lineptr;
  std::ptrdiff_t m_lineindex;
  int            m_width;
  int            m_y;
};



template <class T, class E>
class pixel_range2d_outer : public ranges::view_facade<pixel_range2d_outer<T, E>, ranges::finite>
{
  friend ranges::range_access;

  struct cursor
  {
    auto read() const { return pixel_range2d_inner<T, E>(m_image, m_buffer + m_y * m_stride,  m_index + m_y * m_stride, m_width, m_y); }
    bool equal(const cursor& other) const { return m_y == other.m_y; }
    void next() { m_y++; }

    cursor() = default;
    cursor(E* image, T* buffer, std::ptrdiff_t index,  std::ptrdiff_t stride, int width, int y)
      : m_image(image), m_buffer(buffer), m_index(index), m_stride(stride), m_width(width), m_y(y)
      {
      }

  private:
    E*             m_image;
    T*             m_buffer;
    std::ptrdiff_t m_index;
    std::ptrdiff_t m_stride;
    int            m_width;
    int            m_y;
  };

  cursor begin_cursor() const { return cursor(m_image, m_buffer, m_index, m_stride, m_width, 0);  }
  cursor end_cursor() const { return cursor(nullptr, nullptr, 0, 0, 0, m_height);  }

public:
  pixel_range2d_outer() = default;
  pixel_range2d_outer(E* image, T* buffer, std::ptrdiff_t index, int width, int height, std::ptrdiff_t stride)
    : m_image(image), m_buffer(buffer), m_index(index), m_stride(stride), m_width(width), m_height(height)
    {
    }

private:
  E*                    m_image;
  T*                    m_buffer;
  std::ptrdiff_t        m_index;
  std::ptrdiff_t        m_stride;
  int                   m_width;
  int                   m_height;
};





template <class T>
pixel_range2d_outer<const T, const mln::image2d<T>> pixels_of(const mln::image2d<T>& ima)
{
  std::ptrdiff_t stride = ima.index_strides()[0];
  int width = ima.ncols();
  int height = ima.nrows();
  const T* buffer = &(ima.at(0,0));
  std::ptrdiff_t index = ima.index_of_point({0,0});
  return pixel_range2d_outer<const T, const mln::image2d<T>>(&ima, buffer, index, width, height, stride);
}

template <class T>
pixel_range2d_outer<T, mln::image2d<T>> pixels_of(mln::image2d<T>& ima)
{
  std::ptrdiff_t stride = ima.index_strides()[0];
  int width = ima.ncols();
  int height = ima.nrows();
  T* buffer = &(ima.at(0,0));
  std::ptrdiff_t index = ima.index_of_point({0,0});
  return pixel_range2d_outer<T, mln::image2d<T>>(&ima, buffer, index, width, height, stride);
}





