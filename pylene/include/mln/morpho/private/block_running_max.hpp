#pragma once

#include <cstddef>
#include <mln/morpho/private/block_algorithm.hpp>



namespace mln::morpho::details
{

  // Apply the dilation column wise
  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  void block_running_max_inplace(T* in, int width, int height, std::ptrdiff_t byte_stride, int k, BinaryFunction sup);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/



  class block_running_max_algo_base
  {
  protected:
    virtual void partial_sum(const std::byte* in, std::byte* out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, void* sup) = 0;
    virtual void transform(const std::byte* a, const std::byte* b, std::byte* out, int width, int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t c_byte_stride, void* sup) = 0;
    void run(int block_width, int e_size, std::byte* in, int width, int height, std::ptrdiff_t byte_stride, int k, void* sup);
  };

  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  class block_running_max_algo : public block_running_max_algo_base
  {
  private:
    void partial_sum(const std::byte* in, std::byte* out, int width, int height, std::ptrdiff_t in_byte_stride,
                     std::ptrdiff_t out_byte_stride, void* sup) final
    {
      block_partial_sum<BLOCK_WIDTH>((const T*)in, (T*)out, width, height, in_byte_stride, out_byte_stride, *(BinaryFunction*)sup);
    }

    void transform(const std::byte* a, const std::byte* b, std::byte* out, int width, int height,
                   std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride, void* sup) final
    {
      block_transform<BLOCK_WIDTH>((const T*)a, (const T*)b, (T*)out, width, height, a_byte_stride, b_byte_stride,
                                   out_byte_stride, *(BinaryFunction*)sup);
    }

  public:
    void run(T* in, int width, int height, std::ptrdiff_t byte_stride, int k, BinaryFunction sup)
    {
      block_running_max_algo_base::run(BLOCK_WIDTH, sizeof(T), (std::byte*)in, width, height, byte_stride, k, &sup);
    }

    block_running_max_algo() = default;
  };



  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  void block_running_max(T*  in, int width, int height, std::ptrdiff_t byte_stride, int k, BinaryFunction sup)
  {
    block_running_max_algo<BLOCK_WIDTH, T, BinaryFunction> algo;
    algo.run(in, width, height, byte_stride, k, sup);
  }

}
