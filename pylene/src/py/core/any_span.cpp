#include <py/core/any_span.hpp>
#include <py/core/any_ref.hpp>

#include <exception>
#include <iterator>

namespace mln::py
{
  any_span::cursor::cursor(std::byte* buf, std::size_t stride, Info i)
    : m_ptr{buf}
    , m_stride{stride}
    , m_info{i}
  {
  }

  any_ref any_span::cursor::read() const
  {
    return any_ref(m_ptr, m_info);
  }

  void any_span::cursor::next() { m_ptr += m_stride; }

  void any_span::cursor::prev() { m_ptr -= m_stride; }


  bool any_span::cursor::equal(const any_span::cursor& c) const { return m_ptr == c.m_ptr; }

  any_span::any_span(std::byte* buf, std::size_t sz, Info i)
    : m_buffer{buf}
    , m_size{sz}
    , m_type{i}
  {
  }

  any_span::cursor any_span::begin_cursor() { return any_span::cursor(m_buffer, m_type.dyn_sizeof(), m_type); }

  any_span::cursor any_span::end_cursor() { return any_span::cursor(m_buffer + m_size * m_type.dyn_sizeof(), 0, m_type); }
} // namespace mln::py