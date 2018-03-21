#pragma once

#include "private/ndimage_interleaved.hpp"


namespace mln
{

  template <class V>
  class image3d_interleaved : public ndimage_interleaved<3, V>
  {
  private:
    using base = ndimage_interleaved<3, V>;
  public:
    using domain_type = typename base::domain_type;
    using base::base;

    image3d_interleaved(int nslice, int height, int width, int border = 3);
    image3d_interleaved(int nslice, int height, int width, int border, V val);
    image3d_interleaved(std::initializer_list<std::initializer_list<std::initializer_list<V>>> l, int border = 3);

    image3d_interleaved(ndimage_interleaved<3, V>&&);
    image3d_interleaved(const ndimage_interleaved<3, V>&);

    using base::at;

    V& at(int slice, int row, int col);
    const V& at(int slice, int row, int col) const;

    static image3d_interleaved from_buffer(void* buffer, const domain_type& domain);
    static image3d_interleaved from_buffer(void* buffer, const domain_type& domain, const std::size_t* byte_strides);

    int nslices() const;
    int nrows() const;
    int ncols() const;
    int width() const;
    int height() const;
  };

  template <class V>
  using image3d = image3d_interleaved<V>;

  /******************************************/
  /****             Traits               ****/
  /******************************************/

  template <class V>
  struct image_traits<image3d_interleaved<V>> : image_traits<ndimage_interleaved<3, V>>
  {
  };

  template <class V>
  struct image_concrete<image3d_interleaved<V>>
  {
    using type = ndimage_interleaved<3, V>;
  };


  template <class V, class U>
  struct image_ch_value<image3d_interleaved<V>, U>
  {
    using type = ndimage_interleaved<3, U>;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class V>
  image3d_interleaved<V>::image3d_interleaved(ndimage_interleaved<3, V>&& other)
    : base(std::move(other))
  {
  }

  template <class V>
  image3d_interleaved<V>::image3d_interleaved(const ndimage_interleaved<3, V>& other)
    : base(other)
  {
  }


  template <class V>
  image3d_interleaved<V>::image3d_interleaved(int slice, int height, int width, int border)
    : base(domain_type{{0,0,0}, {slice, height, width}}, border)
  {
  }

  template <class V>
  image3d_interleaved<V>::image3d_interleaved(int slice, int height, int width, int border, V val)
    : base(domain_type{{0,0,0}, {slice, height, width}}, border, val)
  {
  }

  template <class V>
  image3d_interleaved<V>::image3d_interleaved(std::initializer_list<std::initializer_list<std::initializer_list<V>>> l,
                                              int border)
  {
    mln_precondition(l.size() != 0);
    mln_precondition(l.begin()->size() != 0);
    mln_precondition(l.begin()->begin()->size() != 0);

    int ns = static_cast<int>(l.size());
    int nr = static_cast<int>(l.begin()->size());
    int nc = static_cast<int>(l.begin()->begin()->size());
    this->resize(domain_type{{0, 0, 0}, {ns, nr, nc}}, border);

    mln_iter(v, this->values());
    v.init();
    for (const auto& slice : l)
    {
      mln_assertion(static_cast<int>(slice.size()) == nr);
      for (const auto& row : slice)
      {
        mln_assertion(static_cast<int>(row.size()) == nc);
        for (const V *val = row.begin(); val != row.end(); ++val, v.next())
          *v = *val;
      }
    }
  }

  template <class V>
  V& image3d_interleaved<V>::at(int slice, int row, int col)
  {
    return base::at({slice, row, col});
  }

  template <class V>
  const V& image3d_interleaved<V>::at(int slice, int row, int col) const
  {
    return base::at({slice, row, col});
  }

  template <class V>
  int image3d_interleaved<V>::nslices() const
  {
    return this->dim(0);
  }


  template <class V>
  int image3d_interleaved<V>::height() const
  {
    return this->dim(1);
  }

  template <class V>
  int image3d_interleaved<V>::nrows() const
  {
    return this->dim(1);
  }

  template <class V>
  int image3d_interleaved<V>::width() const
  {
    return this->dim(2);
  }

  template <class V>
  int image3d_interleaved<V>::ncols() const
  {
    return this->dim(2);
  }

  template <class V>
  image3d_interleaved<V>
  image3d_interleaved<V>::from_buffer(void* buffer, const domain_type& domain)
  {
    image3d_interleaved f;
    f.__from_buffer(buffer, domain);
    return f;
  }

  template <class V>
  image3d_interleaved<V>
  image3d_interleaved<V>::from_buffer(void* buffer, const domain_type& domain, const std::size_t byte_strides[])
  {
    image3d_interleaved f;
    f.__from_buffer(buffer, domain, byte_strides);
    return f;
  }

}

