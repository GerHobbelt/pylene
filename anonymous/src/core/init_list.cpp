#include <ano/core/private/init_list.hpp>

#include <cassert>
#include <cstddef>
#include <functional>
#include <stdexcept>

namespace ano::details
{

  namespace
  {
    void copy_to_buffer_impl(const std::initializer_list<std::initializer_list<std::initializer_list<std::byte>>>& data,
                             std::function<void(const void*, std::size_t, void*)> cpy, std::byte* buffer,
                             const std::size_t* sizes, std::ptrdiff_t line_byte_stride,
                             std::ptrdiff_t slice_byte_stride)
    {
      const std::size_t depth  = sizes[2];
      const std::size_t height = sizes[1];
      const std::size_t width  = sizes[0];


      auto        current_slice = data.begin();
      std::size_t rowsize       = current_slice->begin()->size();
      for (std::size_t z = 0; z < depth; ++z)
      {
        if (current_slice->size() != height)
          throw std::runtime_error("Initialization list has an invalid shape.");

        auto current_line = current_slice->begin();
        auto lineptr      = buffer;
        for (std::size_t y = 0; y < height; ++y)
        {
          if (current_line->size() != rowsize)
            throw std::runtime_error("A line has an invalid width");
          cpy(current_line->begin(), width, lineptr);
          ++current_line;
          lineptr += line_byte_stride;
        }
        ++current_slice;
        buffer += slice_byte_stride;
      }
    }
  } // namespace


  void init_list::copy_to_buffer(void* buffer, std::ptrdiff_t line_byte_stride, std::ptrdiff_t slice_byte_stride) const
  {
    using init_list_3_t = std::initializer_list<std::initializer_list<std::initializer_list<std::byte>>>;
    using init_list_2_t = std::initializer_list<std::initializer_list<std::byte>>;
    using init_list_1_t = std::initializer_list<std::byte>;


    if (m_dim == 1)
    {
      auto          tmp  = reinterpret_cast<const init_list_1_t*>(m_ptr);
      init_list_3_t data = {{*tmp}};
      copy_to_buffer_impl(data, m_cpy, static_cast<std::byte*>(buffer), m_sizes, line_byte_stride, slice_byte_stride);
    }
    else if (m_dim == 2)
    {
      auto          tmp  = reinterpret_cast<const init_list_2_t*>(m_ptr);
      init_list_3_t data = {*tmp};
      copy_to_buffer_impl(data, m_cpy, static_cast<std::byte*>(buffer), m_sizes, line_byte_stride, slice_byte_stride);
    }
    else if (m_dim == 3)
    {
      auto data = reinterpret_cast<const init_list_3_t*>(m_ptr);
      copy_to_buffer_impl(*data, m_cpy, static_cast<std::byte*>(buffer), m_sizes, line_byte_stride, slice_byte_stride);
    }
    else
    {
      assert(false && "Unexpected number of dimensions.");
    }
  }
} // namespace ano::details
