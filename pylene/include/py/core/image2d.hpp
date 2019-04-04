#include <byte>
#include <cstddef>
#include <iostream>
#include <memory>
#include <py/core/image2d_data.hpp>
#include <py/core/type_info.hpp>
#include <range/v3/span.hpp>

template <typename T = void>
class image2d;

template <>
class image2d<void>
{
  using data_t = std::shared_ptr<image2d_data_base>;

public:
  image2d() = default;
  image2d(std::size_t w, std::size_t h, Info::type_id type)
    : m_width{w}
    , m_height{h}
    , m_type{type}
  {
    m_data   = std::make_shared<image2d_data<void>>(h * w, m_type.val);
    m_buffer = m_data->m_buf;
    std::cout << "initialized a non-templated image\n";
  }

  template <typename T>
  image2d(std::size_t w, std::size_t h, Tag<T>)
    : m_width{w}
    , m_height{h}
    , m_type{Trait<T>::value}
  {
    m_data   = std::make_shared<image2d_data<T>>(h * w);
    m_buffer = m_data->m_buf;
    std::cout << "initialized a templated img\n";
  }

  template <typename T>
  image2d(std::size_t w, std::size_t h, T val)
    : m_width{w}
    , m_height{h}
    , m_type{Trait<T>::value}
  {
    m_data   = std::make_shared<image2d_data<T>>(h * w, val);
    m_buffer = m_data->m_buf;
    std::cout << "initialized a templated img with values\n";
  }

  static image2d from_buffer(std::byte* buffer, std::size_t w, std::size_t h, Info::type_id type)
  {
    auto res     = image2d(w, h, type);
    res.m_buffer = buffer;
    return res;
  }

  size_t     width() const { return m_width; }
  size_t     height() const { return m_height; }
  std::byte* buffer() const { return m_buffer; }
  Info       type() const { return m_type; }

protected:
  std::byte* m_buffer; // buffer contained by m_data

private:
  size_t m_width  = 0;
  size_t m_height = 0;
  Info   m_type   = {Info::OTHER}; // info for the type the data
  data_t m_data;                   // ptr to the data
};

template <typename T>
class image2d : public image2d<void>
{
public:
  image2d(std::size_t w, std::size_t h)
    : image2d<void>(w, h, Tag<T>{})
  {
  }

  image2d(std::size_t w, std::size_t h, T val)
    : image2d<void>(w, h, val)
  {
  }

  T* buffer() const { return reinterpret_cast<T*>(this->image2d<void>::buffer()); }

  ranges::span<T> values() { return ranges::span(buffer(), width() * height()); }

  ranges::span<const T> values() const { return ranges::span(buffer(), width() * height()); }

  static image2d<T> from_buffer(T* buffer, std::size_t w, std::size_t h)
  {
    auto res     = image2d(w, h);
    res.m_buffer = reinterpret_cast<std::byte*>(buffer);
    return res;
  }
};
