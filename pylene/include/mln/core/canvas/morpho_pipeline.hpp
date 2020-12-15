#pragma once

#include <any>
#include <functional>

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>

namespace mln::morpho
{
  namespace details
  {
    template <class T, class enable = void>
    struct grad_op
    {
      using result_type = decltype(l2norm(std::declval<T>()));

      result_type operator()(T a, T b) const noexcept { return l2norm(T(a - b)); }
    };

    template <class T>
    // require std...
    struct grad_op<T, std::enable_if_t<std::is_arithmetic_v<T>>>
    {
      using result_type = T;

      T operator()(T a, T b) const noexcept { return a - b; }
    };

    template <class I>
    using gradient_result_t = image_ch_value_t<I, typename grad_op<image_value_t<I>>::result_type>;
  }

  enum class e_MorphoPipelineOperation
  {
    Closing,
    Opening,
    Grad_thick,
    Grad_ext,
    Grad_int,
    Top_hat,
    Bot_hat
  };

  class MorphoPipeline
  {
    using functype         = std::function<std::any(std::any&)>;
    using functype_out     = std::function<void(std::any&, std::any&)>;
    using last_op_functype = std::function<void(std::any&, std::any&, std::any&)>;

  public:

    template <class Image, class SE, class OutputImage>
    MorphoPipeline(e_MorphoPipelineOperation op, Image& input, const SE& se, OutputImage& out)
      : m_input{input}
      , m_output{out}
      , m_op{op}
    {
      m_erode = [se](std::any& f) -> std::any {
        return mln::morpho::parallel::erosion(std::any_cast<Image&>(f), se);
      };
      m_dilate = [se](std::any& f) -> std::any {
        return mln::morpho::parallel::dilation(std::any_cast<Image&>(f), se);
      };
      m_erode_out = [se](std::any& f, std::any& out) {
        return mln::morpho::parallel::erosion(std::any_cast<Image&>(f), se, std::any_cast<OutputImage&>(out));
      };
      m_dilate_out = [se](std::any& f, std::any& out) {
        return mln::morpho::parallel::dilation(std::any_cast<Image&>(f), se, std::any_cast<OutputImage&>(out));
      };
      m_diff = [](std::any& a, std::any& b, std::any& out) {
        using I = std::remove_reference_t<Image>;
        mln::parallel::transform(std::any_cast<Image&>(a), std::any_cast<Image&>(b), std::any_cast<OutputImage&>(out), details::grad_op<image_value_t<I>>());
      };
    }


    void execute();


  private:
    std::any                  m_input;
    std::any                  m_output;
    functype                  m_erode;
    functype                  m_dilate;
    functype_out              m_erode_out;
    functype_out              m_dilate_out;
    last_op_functype          m_diff;
    e_MorphoPipelineOperation m_op;
  };


} // namespace mln::morpho
