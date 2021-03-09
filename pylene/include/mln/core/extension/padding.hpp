#pragma once

#include <cstddef>
#include <algorithm>

#include <mln/core/image/ndimage_fwd.hpp>
#include <mln/core/range/mdspan_fwd.hpp>
#include <mln/core/box.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/bp/tile.hpp>

namespace mln
{
  enum e_padding_mode
  {
    PAD_ZERO,
    PAD_CONSTANT,
    PAD_WRAP,
    PAD_MIRROR,
    PAD_REPLICATE,
  };

  /// \brief Pad an ndimage or a mdspan in-place using one of the padding strategy
  ///
  /// Given `[0 0 1 2 3 0 0 0 0]`, borders = `{2, 4}` and value = `9`
  ///
  /// PAD_ZERO
  ///   pad each dimension with value zero `[0 0 1 2 3 0 0 0 0]`
  /// PAD_CONSTANT
  ///   pad each dimension with \p value `[9 9 1 2 3 9 9 9 9]`
  /// PAD_WRAP
  ///   pad each dimension using periodization `[2 3 1 2 3 1 2 3 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge mirroring `[2 1 1 2 3 3 2 1 1]`
  /// PAD_REPLICATE
  ///   pad each dimension using edge replication `[1 1 1 2 3 3 3 3 3]`
  ///
  /// \param input The image or mdspan to pad
  /// \param mode The padding mode
  /// \param borders The padding region express as distances from the border
  /// \param value The value used when mode=PAD_CONSTANT
  template <class T, int dim>
  void pad(ndimage<T, dim>& image, e_padding_mode mode, const int borders[][2], T value = {});

  /// \overload
  template <concepts::Image I>
  void pad(I image, e_padding_mode mode, const int borders[][2], image_value_t<I> value = {});

  /// \overload
  template <class T, std::size_t dim>
  void pad(const ranges::mdspan<T, dim>& image, e_padding_mode mode, const int borders[][2], T value = {});

  /// \overload
  template <class T>
  void pad(const Tile2DView<T>& image, e_padding_mode mode, const int borders[][2], T value = {});




  /// \brief Copy a region from a ndimage or a mdspan and pad the missing values using one of the padding strategy
  ///
  /// Given the input `[1 2 3]` defined on domain [0-3] and the output defined on domain [-2, 7] and value = `9`. The result in output is:
  ///
  /// PAD_ZERO
  ///   pad each dimension with value zero `[0 0 1 2 3 0 0 0 0]`
  /// PAD_CONSTANT
  ///   pad each dimension with \p value `[9 9 1 2 3 9 9 9 9]`
  /// PAD_WRAP
  ///   pad each dimension using periodization `[2 3 1 2 3 1 2 3 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge mirroring `[2 1 1 2 3 3 2 1 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge replication `[1 1 1 2 3 3 3 3 3]`
  ///
  /// \param input The input mdspan defined on [0...sizes] (used as referencial)
  /// \param out The output buffer
  /// \param roi Region of the output buffer (coordinates expressed in the reference space)
  /// \param mode The padding mode
  /// \param value The value used when mode=PAD_CONSTANT
  template <class T, int dim>
  void copy_pad(ranges::mdspan<T, dim> input, ranges::mdspan<T, dim> out, mln::ndbox<dim> roi, e_padding_mode mode,  T value = {});

  /// \overload
  template <class T, int dim>
  void copy_pad(const ndimage<T, dim>& input, ndimage<T, dim>& output, e_padding_mode mode,  T value = {});

  /// \overload
  template <concepts::Image I, concepts::Image J>
  void copy_pad(I in, J out, e_padding_mode mode, image_value_t<J> value = {});


  /// \brief Transpose a region from a 2D image or a 2D-span and pad the missing values using one of the padding strategy
  ///
  /// Given the input `[1 2 3]` defined on domain [0-3] and the output defined on domain [-2, 7] and value = `9`. The result in output is:
  ///
  /// PAD_ZERO
  ///   pad each dimension with value zero `[0 0 1 2 3 0 0 0 0]`
  /// PAD_CONSTANT
  ///   pad each dimension with \p value `[9 9 1 2 3 9 9 9 9]`
  /// PAD_WRAP
  ///   pad each dimension using periodization `[2 3 1 2 3 1 2 3 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge mirroring `[2 1 1 2 3 3 2 1 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge replication `[1 1 1 2 3 3 3 3 3]`
  ///
  /// \param input The input mdspan defined on [0...sizes] (used as referencial)
  /// \param out The output buffer
  /// \param roi Region of the output buffer (coordinates expressed in the reference space)
  /// \param mode The padding mode
  /// \param value The value used when mode=PAD_CONSTANT
  template <concepts::Image I, concepts::Image J>
  void transpose_pad(I input, J out, e_padding_mode mode, image_value_t<J> value = {});




  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T>
  class padder;


  template <>
  class padder<void>
  {
  public:
    virtual void memset(void* buffer, std::size_t n, const void* value) const noexcept = 0;
    virtual void memcpy(void* dst, const void* src, std::size_t n) const noexcept = 0;
    virtual void mirror(void* dst, const void* src, int a, int b, int width) const noexcept = 0;
    virtual void periodize(void* dst, const void* src, int a, int b, int width) const noexcept = 0;
  };



  template <class T>
  class padder : public padder<void>
  {
  public:
    void memset(void* buffer, std::size_t n, const void* value) const noexcept final
    {
      T val = value ? *(const T*)value : 0;
      std::fill_n(reinterpret_cast<T*>(buffer), n, val);
    }


    void memcpy(void* dst, const void* src, std::size_t n) const noexcept { std::copy_n((T*)src, n, (T*)dst); }


    void mirror(void* dst_, const void* src_, int a, int b, int w) const noexcept final
    {
      auto mirror = [w](int x) {
        int n = w * 2;
        x     = x % n;                     // x in (-n:n)
        x     = (x >= 0) ? x : x + n;      // x in [0:n)
        x     = (x < w) ? x : (n - x - 1); // x in [0,w)
        return x;
      };

      const T* src  = reinterpret_cast<const T*>(src_);
      T*       dest = reinterpret_cast<T*>(dst_);

      for (int x = a; x < b; ++x)
        *dest++ = src[mirror(x)];
    }

    void periodize(void* dst_, const void* src_, int a, int b, int w) const noexcept final
    {
      auto periodize = [w](int x) {
        x = x % w;
        x = (x >= 0) ? x : x + w;
        return x;
      };

      const T* src  = reinterpret_cast<const T*>(src_);
      T*       dest = reinterpret_cast<T*>(dst_);

      for (int x = a; x < b; ++x)
        *dest++ = src[periodize(x)];
    }
  };

  namespace impl
  {
    void copy_pad(mln::padder<void>* vs, int dim, std::byte* dst, std::byte* src, mln::Box roi, const int sizes[],
                  const std::ptrdiff_t dst_strides[], const std::ptrdiff_t src_strides[], mln::e_padding_mode padding,
                  void* value) noexcept;

    void pad(mln::padder<void>* vs, int dim, std::byte* buffer, const int sizes[], const std::ptrdiff_t strides[],
             const int borders[][2], e_padding_mode padding, void* value) noexcept;
  }




  template <class T, std::size_t dim>
  void pad(const ranges::mdspan<T, dim>& sp, e_padding_mode mode, const int borders[][2], T value)
  {
    padder<T>      _padder;
    std::ptrdiff_t strides[dim];
    int            sizes[dim];

    for (int i = 0; i < static_cast<int>(dim); ++i)
    {
      sizes[i] = static_cast<int>(sp.size(i));
      strides[i] = sp.byte_stride(i);
    }

    T* vptr = (mode == mln::PAD_ZERO) ? nullptr : &value;
    impl::pad(&_padder, static_cast<int>(dim), (std::byte*)sp.data(), sizes, strides, borders, mode, vptr);

  }

  template <class T>
  void pad(Tile2DView<T> f, e_padding_mode mode, const int borders[][2], T value)
  {
    padder<T>      _padder;
    std::ptrdiff_t strides[2] = {sizeof(T), f.stride()};
    int            sizes[2] = {f.width(), f.height()};

    T* vptr = (mode == mln::PAD_ZERO) ? nullptr : &value;
    impl::pad(&_padder, 2, (std::byte*)f.buffer(), sizes, strides, borders, mode, vptr);
  }


  template <class T, int dim>
  void pad(ndimage<T, dim>& f, e_padding_mode mode, const int borders[][2], T value)
  {
    padder<T>      _padder;
    std::ptrdiff_t strides[dim];
    int            sizes[dim];

    for (int i = 0; i < dim; ++i)
    {
      sizes[i] = static_cast<int>(f.size(i));
      strides[i] = f.byte_stride(i);
    }

    T* vptr = (mode == mln::PAD_ZERO) ? nullptr : &value;
    impl::pad(&_padder, dim, (std::byte*)f.buffer(), sizes, strides, borders, mode, vptr);
  }





  template <class T, int dim>
  void copy_pad(ranges::mdspan<T, dim> input, ranges::mdspan<T, dim> out, mln::ndbox<dim> roi, e_padding_mode mode,  T value)
  {
    padder<T>      _padder;

    std::ptrdiff_t istrides[dim];
    std::ptrdiff_t ostrides[dim];
    int            sizes[dim];

    for (int i = 0; i < static_cast<int>(dim); ++i)
    {
      assert((std::size_t)roi.size(i) == (std::size_t)out.size(i));

      sizes[i] = static_cast<int>(input.size(i));
      istrides[i] = input.byte_stride(i);
      ostrides[i] = out.byte_stride(i);
    }

    T* vptr = (mode == mln::PAD_ZERO) ? nullptr : &value;
    impl::copy_pad(&_padder, dim, (std::byte*)out.data(), (std::byte*)input.data(), roi, sizes, ostrides, istrides, mode, vptr);
  }


  template <class T, int dim>
  void copy_pad(const ndimage<T, dim>& input, ndimage<T, dim>& output, e_padding_mode mode,  T value)
  {
    padder<T>      _padder;

    std::ptrdiff_t  istrides[dim];
    std::ptrdiff_t  ostrides[dim];
    int             sizes[dim];
    mln::ndbox<dim> roi = output.domain();
    auto            x0  = input.domain().tl();


    for (int i = 0; i < dim; ++i)
    {
      assert((std::size_t)roi.size(i) == (std::size_t)output.size(i));

      sizes[i]    = static_cast<int>(input.size(i));
      istrides[i] = input.byte_stride(i);
      ostrides[i] = output.byte_stride(i);
      roi.tl()[i] -= x0[i];
      roi.br()[i] -= x0[i];
    }

    T* vptr = (mode == mln::PAD_ZERO) ? nullptr : &value;
    impl::copy_pad(&_padder, dim, (std::byte*)output.buffer(), (std::byte*)input.buffer(), roi, sizes, ostrides, istrides, mode, vptr);


  }




  namespace impl
  {
    template <class I, typename T, int dim>
    void pad_generic(I image, e_padding_mode mode, const int borders[][2], T value, mln::ndbox<dim> domain)
    {
      ndbox<dim> inner;

      for (int d = 0; d < dim; ++d)
      {
        inner.tl()[d] = domain.tl()[d] + borders[d][0];
        inner.br()[d] = domain.br()[d] - borders[d][1];
        if (inner.tl()[d] >= inner.br()[d])
          return;
      }

      mln_foreach(auto p, domain)
        if (!inner.has(p))
          switch (mode)
          {
          case PAD_ZERO: image(p) = 0;
            break;
          case PAD_CONSTANT: image(p) = value;
            break;
          case PAD_WRAP: image(p) = image(inner.periodize(p));
            break;
          case PAD_MIRROR: image(p) = image(inner.mirror(p));
            break;
          case PAD_REPLICATE: image(p) = image(inner.clamp(p));
            break;
          }
    }


    template <concepts::Image I, concepts::Image J, typename T, int dim>
    void copy_pad_generic(I in, J out, e_padding_mode mode, T value, mln::ndbox<dim> domain)
    {
      ndbox<dim> inner  = in.domain();

      mln_foreach (auto p, domain)
        if (inner.has(p))
          out(p) = in(p);
        else
          switch (mode)
          {
          case PAD_ZERO:
            out(p) = 0;
            break;
          case PAD_CONSTANT:
            out(p) = value;
            break;
          case PAD_WRAP:
            out(p) = in(inner.periodize(p));
            break;
          case PAD_MIRROR:
            out(p) = in(inner.mirror(p));
            break;
          case PAD_REPLICATE:
            out(p) = in(inner.clamp(p));
            break;
          }
    }


    template <concepts::Image I, concepts::Image J, typename T>
    void transpose_pad_generic(I in, J out, e_padding_mode mode, T value, mln::box2d domain)
    {
      auto inner = in.domain();

      mln::box2d dom_T(domain.y(), domain.x(), domain.height(), domain.width());

      mln_foreach (auto p, dom_T)
      {
        point2d q = {p.y(), p.x()};

        if (inner.has(p))
          out(q) = in(p);
        else
          switch (mode)
          {
          case PAD_ZERO:
            out(q) = 0;
            break;
          case PAD_CONSTANT:
            out(q) = value;
            break;
          case PAD_WRAP:
            out(q) = in(inner.periodize(p));
            break;
          case PAD_MIRROR:
            out(q) = in(inner.mirror(p));
            break;
          case PAD_REPLICATE:
            out(q) = in(inner.clamp(p));
            break;
          }
      }
    }

  } // namespace impl


  template <concepts::Image I>
  void pad(I image, e_padding_mode mode, const int borders[][2], image_value_t<I> value)
  {
    static_assert(std::is_convertible_v<image_domain_t<I>, mln::Box>);
    impl::pad_generic(std::move(image), mode, borders, value, image.domain());
  }


  template <concepts::Image I, concepts::Image J>
  void copy_pad(I in, J out, e_padding_mode mode, image_value_t<J> value)
  {
    static_assert(std::is_convertible_v<image_domain_t<I>, mln::Box>);
    static_assert(std::is_convertible_v<image_domain_t<J>, mln::Box>);

    impl::copy_pad_generic(std::move(in), std::move(out), mode, value, out.domain());
  }

  template <concepts::Image I, concepts::Image J>
  void transpose_pad(I input, J out, e_padding_mode mode,  image_value_t<J> value)
  {
    impl::transpose_pad_generic(std::move(input), std::move(out), mode, value, out.domain());
  }



} // namespace mln
