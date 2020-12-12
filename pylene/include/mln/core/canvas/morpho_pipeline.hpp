#pragma once

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
    using functype         = std::function<mln::ndbuffer_image(mln::ndbuffer_image)>;
    using last_op_functype = std::function<mln::ndbuffer_image(mln::ndbuffer_image, mln::ndbuffer_image)>;

  public:
    template <class Image, class SE>
    MorphoPipeline(e_MorphoPipelineOperation op, Image& input, const SE& se)
      : m_input{input}
      , m_op{op}
    {
      m_erode = [se](mln::ndbuffer_image f) -> mln::ndbuffer_image {
        return mln::morpho::parallel::erosion(static_cast<Image&>(f), se);
      };
      m_dilate = [se](mln::ndbuffer_image f) -> mln::ndbuffer_image {
        return mln::morpho::parallel::dilation(static_cast<Image&>(f), se);
      };
      m_diff = [](mln::ndbuffer_image a, mln::ndbuffer_image b) -> mln::ndbuffer_image {
        using I = std::remove_reference_t<Image>;
        return mln::transform(static_cast<Image&>(a), static_cast<Image&>(b), details::grad_op<image_value_t<I>>());
      }; // TODO parallel
    }

    template <class Image, class SE, class OutputImage>
    MorphoPipeline(e_MorphoPipelineOperation op, Image& input, const SE& se, OutputImage& out)
      : m_input{input}
      , m_output{out}
      , m_op{op}
    {
      m_erode = [se](mln::ndbuffer_image f) -> mln::ndbuffer_image {
        return mln::morpho::parallel::erosion(static_cast<Image&>(f), se);
      };
      m_dilate = [se](mln::ndbuffer_image f) -> mln::ndbuffer_image {
        return mln::morpho::parallel::dilation(static_cast<Image&>(f), se);
      };
      m_diff = [](mln::ndbuffer_image a, mln::ndbuffer_image b) -> mln::ndbuffer_image {
        using I = std::remove_reference_t<Image>;
        return mln::transform(static_cast<Image&>(a), static_cast<Image&>(b), details::grad_op<image_value_t<I>>());
      }; // TODO parallel
    }

    ndbuffer_image execute() const;
    void execute_inplace();


  private:
    ndbuffer_image            m_input;
    ndbuffer_image            m_output;
    functype                  m_erode;
    functype                  m_dilate;
    last_op_functype          m_diff;
    e_MorphoPipelineOperation m_op;
  };


} // namespace mln::morpho
