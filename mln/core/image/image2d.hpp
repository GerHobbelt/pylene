#pragma once

#include "private/ndimage_interleaved.hpp"


namespace mln
{

  template <class V>
  class image2d_interleaved : public ndimage_interleaved<2, V>
  {
  private:
    using base = ndimage_interleaved<2, V>;
  public:
    using domain_type = typename base::domain_type;
    using base::base;

    image2d_interleaved(int height, int width, int border = 3);
    image2d_interleaved(int height, int width, int border, V val);
    image2d_interleaved(std::initializer_list<std::initializer_list<V>> l, int border = 3);

    image2d_interleaved(ndimage_interleaved<2, V>&&);
    image2d_interleaved(const ndimage_interleaved<2, V>&);

    using base::at;

    V& at(int row, int col);
    const V& at(int row, int col) const;

    static image2d_interleaved from_buffer(void* buffer, const domain_type& domain);
    static image2d_interleaved from_buffer(void* buffer, const domain_type& domain, const std::size_t* byte_strides);

    int nrows() const;
    int ncols() const;
    int width() const;
    int height() const;
  };

  template <class V>
  using image2d = image2d_interleaved<V>;

  /******************************************/
  /****             Traits               ****/
  /******************************************/

  template <class V>
  struct image_traits<image2d_interleaved<V>> : image_traits<ndimage_interleaved<2, V>>
  {
  };

  template <class V>
  struct image_concrete<image2d_interleaved<V>>
  {
    using type = ndimage_interleaved<2, V>;
  };


  template <class V, class U>
  struct image_ch_value<image2d_interleaved<V>, U>
  {
    using type = ndimage_interleaved<2, U>;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class V>
  image2d_interleaved<V>::image2d_interleaved(ndimage_interleaved<2, V>&& other)
    : base(std::move(other))
  {
  }

  template <class V>
  image2d_interleaved<V>::image2d_interleaved(const ndimage_interleaved<2, V>& other)
    : base(other)
  {
  }


  template <class V>
  image2d_interleaved<V>::image2d_interleaved(int height, int width, int border)
    : base(domain_type{{0,0}, {height, width}}, border)
  {
  }

  template <class V>
  image2d_interleaved<V>::image2d_interleaved(int height, int width, int border, V val)
    : base(domain_type{{0,0}, {height, width}}, border, val)
  {
  }

  template <class V>
  image2d_interleaved<V>::image2d_interleaved(std::initializer_list<std::initializer_list<V>> l, int border)
  {
    mln_precondition(l.size() != 0);
    mln_precondition((l.begin())->size() != 0);

    int nr = static_cast<int>(l.size());
    int nc = static_cast<int>(l.begin()->size());
    this->resize(domain_type{{0, 0}, {nr, nc}}, border);

    mln_iter(v, this->values());
    v.init();
    for (const std::initializer_list<V>& row : l)
    {
      mln_assertion(static_cast<int>(row.size()) == nc);
      for (const V *val = row.begin(); val != row.end(); ++val, v.next())
        *v = *val;
    }
  }

  template <class V>
  V& image2d_interleaved<V>::at(int row, int col)
  {
    return base::at({row, col});
  }

  template <class V>
  const V& image2d_interleaved<V>::at(int row, int col) const
  {
    return base::at({row, col});
  }

  template <class V>
  int image2d_interleaved<V>::height() const
  {
    return this->dim(0);
  }

  template <class V>
  int image2d_interleaved<V>::nrows() const
  {
    return this->dim(0);
  }

  template <class V>
  int image2d_interleaved<V>::width() const
  {
    return this->dim(1);
  }

  template <class V>
  int image2d_interleaved<V>::ncols() const
  {
    return this->dim(1);
  }

  template <class V>
  image2d_interleaved<V>
  image2d_interleaved<V>::from_buffer(void* buffer, const domain_type& domain)
  {
    image2d_interleaved f;
    f.__from_buffer(buffer, domain);
    return f;
  }

  template <class V>
  image2d_interleaved<V>
  image2d_interleaved<V>::from_buffer(void* buffer, const domain_type& domain, const std::size_t byte_strides[])
  {
    image2d_interleaved f;
    f.__from_buffer(buffer, domain, byte_strides);
    return f;
  }

}

