#pragma once

#include <cstddef>
#include <mln/morpho/private/block_algorithm.hpp>



namespace mln::morpho::details
{

  // Apply the dilation column wise
  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  void block_running_max(const T* in, T* out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction sup);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/



  class block_running_max_algo_base
  {
  private:
    virtual void partial_sum(const std::byte* in, std::byte* out, , int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, void* sup) = 0;
    virtual void transform(const std::byte* a, const std::byte* b, std::byte* out, int width, int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t c_byte_stride, void* sup) = 0;
    void run(const std::byte* in, std::byte* out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, void* sup);
  };

  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  class block_running_max_algo : block_running_max_algo_base
  {
  private:
    void partial_sum(const std::byte* in, std::byte* out, int width, int height, std::ptrdiff_t in_byte_stride,
                     std::ptrdiff_t out_byte_stride, void* sup) final
    {
      block_partial_sum<BLOCK_WIDTH>((const T*)in, (T*)out, width, height, in_byte_stride, out_byte_stride, *(BinaryFunction*)sup);
    }

    void transform(const std::byte* a, const std::byte* b, std::byte* out, int width, int height,
                   std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride, void* sup) final;
    {
      block_transform<BLOCK_WIDTH>((const T*)a, (const T*)b, (T*)out, width, height, a_byte_stride, b_byte_stride,
                                   out_byte_stride, *(BinaryFunction*)sup);
    }

  public:
    block_running_max_algo() = default;
  };



  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  void block_running_max(const T*  in, T*  out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction sup)
  {
    block_running_max_algo<BLOCK_WIDTH, T, BinaryFunction> algo;
    algo.run(in, out, width, height, in_byte_stride, out_byte_stride, &sup);
  }

}
