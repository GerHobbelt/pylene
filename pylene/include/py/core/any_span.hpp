#pragma once

#include <py/core/type_info.hpp>
#include <py/core/any_ref.hpp>

#include <range/v3/range_access.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view_facade.hpp>

#include <cstddef>
#include <iterator>

namespace mln::py
{
  class any_span : public ::ranges::view_facade<any_span, ::ranges::finite>
  {
  private:
    friend ::ranges::range_access;

    struct cursor
    {
    private:
      std::byte*  m_ptr;
      std::size_t m_stride;
      Info        m_info;

    public:
      cursor() = default;
      cursor(std::byte* buf_start, std::size_t stride, Info i);

      any_ref read() const;
      void  next();
      void  prev();
      bool  equal(const cursor& c) const;
    };

  public:
    any_span() = default;
    any_span(std::byte* buf, std::size_t sz, Info i);

    template <typename T>
    any_span(T* buf, std::size_t sz)
    {
        m_buffer = static_cast<std::byte*>(buf);
        m_size = sz;
        switch (typeid(T))
        {
            case (typeid(int8_t)):
                m_type = Info::INT8_V;
                break;
            case (typeid(int16_t)):
                m_type = Info::INT16_V;
                break;
            case (typeid(int32_t)):
                m_type = Info::INT32_V;
                break;
            case (typeid(int64_t)):
                m_type = Info::INT64_V;
                break;
            case (typeid(uint8_t)):
                m_type = Info::UINT8_V;
                break;
            case (typeid(uint16_t)):
                m_type = Info::UINT16_V;
                break;
            case (typeid(uint32_t)):
                m_type = Info::UINT32_V;
                break;
            case (typeid(uint64_t)):
                m_type = Info::UINT64_V;
                break;
            case (typeid(float)):
                m_type = Info::FLOAT_V;
                break;
            case (typeid(double)):
                m_type = Info::DOUBLE_V;
                break;
            default:
                m_type = Info::OTHER;
                break;
        }
    }

  private:
    cursor begin_cursor();
    cursor end_cursor();

    std::byte*  m_buffer;
    std::size_t m_size;
    Info        m_type;
  };
} // namespace mln::py
