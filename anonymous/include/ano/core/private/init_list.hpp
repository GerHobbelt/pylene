#pragma once
#include <algorithm>
#include <initializer_list>

namespace ano::details
{
  /// \brief This class allows to type-erase the type and the dimension (up to 3) of nested initializer list
  /// and allows to copy its content to a buffer
  class init_list
  {
  public:
    template <class T>
    explicit init_list(const std::initializer_list<T>& l) noexcept;

    template <class T>
    explicit init_list(const std::initializer_list<std::initializer_list<T>>& l) noexcept;

    template <class T>
    explicit init_list(const std::initializer_list<std::initializer_list<std::initializer_list<T>>>& l) noexcept;

    void copy_to_buffer(void* buffer, std::ptrdiff_t line_byte_stride, std::ptrdiff_t slice_byte_stride) const;

    int         dim() const noexcept { return m_dim; }
    std::size_t size(int k) const noexcept { return m_sizes[k]; }
    std::size_t sample_type_size() const noexcept { return m_sample_type_size; }

  private:
    template <class T>
    static void copy_buffer(const void* buffer, std::size_t n, void* obuffer) noexcept;

    using copy_fun_ptr_t = void (*)(const void*, std::size_t, void*);

    int            m_dim;
    std::size_t    m_sizes[3];
    std::size_t    m_sample_type_size;
    const void*    m_ptr; // Type-erased pointer to const std::initializer_list<...>
    copy_fun_ptr_t m_cpy;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T>
  init_list::init_list(const std::initializer_list<T>& l) noexcept
    : m_dim{1}
    , m_sizes{l.size(), 1, 1}
    , m_sample_type_size(sizeof(T))
    , m_ptr{reinterpret_cast<const void*>(&l)}
    , m_cpy{&copy_buffer<T>}
  {
  }


  template <class T>
  init_list::init_list(const std::initializer_list<std::initializer_list<T>>& l) noexcept
    : m_dim{2}
    , m_sizes{l.begin()->size(), l.size(), 1}
    , m_sample_type_size(sizeof(T))
    , m_ptr{reinterpret_cast<const void*>(&l)}
    , m_cpy{&copy_buffer<T>}
  {
  }

  template <class T>
  init_list::init_list(const std::initializer_list<std::initializer_list<std::initializer_list<T>>>& l) noexcept
    : m_dim{3}
    , m_sizes{l.begin()->begin()->size(), l.begin()->size(), l.size()}
    , m_sample_type_size(sizeof(T))
    , m_ptr{reinterpret_cast<const void*>(&l)}
    , m_cpy{&copy_buffer<T>}
  {
  }

  template <class T>
  void init_list::copy_buffer(const void* ibuffer, std::size_t n, void* obuffer) noexcept
  {
    std::copy_n(static_cast<const T*>(ibuffer), n, static_cast<T*>(obuffer));
  }

} // namespace ano::details
