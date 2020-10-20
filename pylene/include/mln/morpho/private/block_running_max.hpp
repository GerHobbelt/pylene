#pragma once

#include <cstddef>
#include <mln/morpho/private/block_algorithm.hpp>



namespace mln::morpho::details
{

  /// Apply the dilation column wise
  ///
  /// \param in Input image. Must be valid for lines [-k:h+k] of width \p width
  template <class T, class BinaryFunction>
  void block_running_max(T* in, int width, int height, std::ptrdiff_t byte_stride, int k, BinaryFunction sup, T zero);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/



  class block_running_max_algo_base
  {
  protected:
    virtual void partial_sum(const std::byte* in, std::byte* out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, void* sup, void* zero) = 0;
    virtual void transform(const std::byte* a, const std::byte* b, std::byte* out, int width, int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t c_byte_stride, void* sup) = 0;
    void run(int e_size, std::byte* in, int width, int height, std::ptrdiff_t byte_stride, int k, void* sup, void* zero);
  };

  template <class T, class BinaryFunction>
  class block_running_max_algo : public block_running_max_algo_base
  {
  private:
    void partial_sum(const std::byte* in, std::byte* out, int width, int height, std::ptrdiff_t in_byte_stride,
                     std::ptrdiff_t out_byte_stride, void* sup, void* zero) final
    {
      block_partial_sum((const T*)in, (T*)out, width, height, in_byte_stride, out_byte_stride,
                        *(BinaryFunction*)sup, *(T*)zero);
    }

    void transform(const std::byte* a, const std::byte* b, std::byte* out, int width, int height,
                   std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride, void* sup) final
    {
      block_transform((const T*)a, (const T*)b, (T*)out, width, height, a_byte_stride, b_byte_stride,
                      out_byte_stride, *(BinaryFunction*)sup);
    }

  public:
    void run(T* in, int width, int height, std::ptrdiff_t byte_stride, int k, BinaryFunction sup, T zero)
    {
      block_running_max_algo_base::run(sizeof(T), (std::byte*)in, width, height, byte_stride, k, &sup, &zero);
    }

    block_running_max_algo() = default;
  };


  template <class T, class BinaryFunction>
  void block_running_max(T* in, int width, int height, std::ptrdiff_t byte_stride, int k, BinaryFunction sup, T zero)
  {
    block_running_max_algo<T, BinaryFunction> algo;
    algo.run(in, width, height, byte_stride, k, sup, zero);
  }

} // namespace mln::morpho::details
