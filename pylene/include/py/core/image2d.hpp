#pragma once

#include <py/core/image2d_data.hpp>
#include <py/core/type_info.hpp>
#include <py/core/any_span.hpp>
#include <py/core/value_set.hpp>

#include <range/v3/span.hpp>

#include <cstddef>
#include <iostream>
#include <memory>
#include <type_traits>


namespace mln::py
{

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
      dispatch_value_set(&m_vs, m_type.val);
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
      new (&m_vs) value_set<T>{};
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
      new (&m_vs) value_set<T>{};
      std::cout << "initialized a templated img with values\n";
    }

    template <typename T>
    image2d<T>* cast_to(void)
    {
        if (Trait<T>::value == m_type.val)
            return static_cast<image2d<T>*>(this);
        return nullptr;
    }

    static image2d from_buffer(std::byte* buffer, std::size_t w, std::size_t h, Info::type_id type)
    {
      auto res     = image2d(w, h, type);
      res.m_buffer = buffer;
      return res;
    }

    any_span values()
    {
        return {m_buffer, m_width * m_height, m_type};
    }

    any_span values() const // TODO propagate constness to const_any_span
    {
        return {m_buffer, m_width * m_height, m_type};
    }


    const value_set<>&      get_value_set() const { return *reinterpret_cast<const value_set<>*>(&m_vs); }
    size_t                  width() const { return m_width; }
    size_t                  height() const { return m_height; }
    std::byte*              buffer() const { return m_buffer; }
    Info                    type() const { return m_type; }

  protected:
    std::byte* m_buffer; // buffer contained by m_data

  private:
    static void dispatch_value_set(void *vs, Info::type_id tid)
    {
        switch (tid)
        {
            case (Info::INT8_V):
                new (vs) value_set<int8_t>();
                break;
            case (Info::INT16_V):
                new (vs) value_set<int16_t>();
                break;
            case (Info::INT32_V):
                new (vs) value_set<int32_t>();
                break;
            case (Info::INT64_V):
                new (vs) value_set<int64_t>();
                break;
            case (Info::UINT8_V):
                new (vs) value_set<uint8_t>();
                break;
            case (Info::UINT16_V):
                new (vs) value_set<uint16_t>();
                break;
            case (Info::UINT32_V):
                new (vs) value_set<uint32_t>();
                break;
            case (Info::UINT64_V):
                new (vs) value_set<uint64_t>();
                break;
            case (Info::FLOAT_V):
                new (vs) value_set<float>();
                break;
            case (Info::DOUBLE_V):
                new (vs) value_set<double>();
                break;
            default:
                throw std::runtime_error("Unhandled data type");
                break;
        }
    }

    std::aligned_storage_t<sizeof(value_set<>), alignof(value_set<>)> m_vs;
    size_t m_width  = 0;
    size_t m_height = 0;
    Info   m_type   = {Info::OTHER}; // info for the type the data
    data_t m_data;                   // ptr to the data
  };

  template <typename T>
  class image2d : public image2d<>
  {
  public:
    image2d(std::size_t w, std::size_t h)
      : image2d<>(w, h, Tag<T>{})
    {
    }

    image2d(std::size_t w, std::size_t h, T val)
      : image2d<>(w, h, val)
    {
    }

    T* buffer() const { return reinterpret_cast<T*>(this->image2d<void>::buffer()); }

    const value_set<T>& get_value_set() const { return *reinterpret_cast<const value_set<T>*>(&m_vs); } //XXX change & to * for performance ?

    ::ranges::span<T> values() { return ::ranges::span(buffer(), width() * height()); }

    ::ranges::span<const T> values() const { return ::ranges::span(buffer(), width() * height()); }

    static image2d<T> from_buffer(T* buffer, std::size_t w, std::size_t h)
    {
      auto res     = image2d(w, h);
      res.m_buffer = reinterpret_cast<std::byte*>(buffer);
      return res;
    }
  };

} // namespace mln::py

