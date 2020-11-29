#pragma once

#include <functional>

#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>

namespace mln::morpho
{
  namespace details
  {
    enum e_morphoFinish
    {
      Subtraction,
      None
    };

    template <typename SE>
    ndbuffer_image identity(ndbuffer_image in, SE se)
    {
      return in;
    }
  } // namespace details

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

  /*
    TODO
    top hat,
    bot hat,
  */

  class MorphoPipeline
  {
    using functype         = std::function<mln::ndbuffer_image(mln::ndbuffer_image)>;
    using last_op_functype = std::function<mln::ndbuffer_image(mln::ndbuffer_image, mln::ndbuffer_image)>;

  public:
    template <class Image, class SE>
    MorphoPipeline(e_MorphoPipelineOperation op, Image& input, const SE& se)
      : m_input{input}
      , m_op(op)
    {
      using V = image_value_t<Image>;
      m_erode = [se](mln::ndbuffer_image f) -> mln::ndbuffer_image {
        return mln::morpho::parallel::erosion(static_cast<Image&>(f), se);
      };
      m_dilate = [se](mln::ndbuffer_image f) -> mln::ndbuffer_image {
        return mln::morpho::parallel::dilation(static_cast<Image&>(f), se);
      };
      m_diff = [](mln::ndbuffer_image a, mln::ndbuffer_image b) -> mln::ndbuffer_image {
        return mln::transform(static_cast<Image&>(a), static_cast<Image&>(b), std::minus<V>());
      }; // TODO parallel
    }

    ndbuffer_image execute() const
    {
      mln::ndbuffer_image dil;
      mln::ndbuffer_image ero;
      switch (m_op)
      {
      case e_MorphoPipelineOperation::Closing:
        dil = m_dilate(m_input);
        return m_erode(dil);
      case e_MorphoPipelineOperation::Opening:
        ero = m_erode(m_input);
        return m_dilate(ero);
      case e_MorphoPipelineOperation::Grad_thick:
        dil = m_dilate(m_input);
        ero = m_erode(m_input);
        return m_diff(dil, ero);
      case e_MorphoPipelineOperation::Grad_ext:
        dil = m_dilate(m_input);
        return m_diff(dil, m_input);
      case e_MorphoPipelineOperation::Grad_int:
        ero = m_erode(m_input);
        return m_diff(m_input, ero);
      case e_MorphoPipelineOperation::Top_hat:
        // FIXME opening(input) - id(input)
        throw std::runtime_error("No implementation found.");
        break;
      case e_MorphoPipelineOperation::Bot_hat:
        throw std::runtime_error("No implementation found.");
        // FIXME id(input) - closing(input)
        break;
      default:
        throw std::runtime_error("No implementation found.");
      }
    }

  private:
    ndbuffer_image            m_input;
    functype                  m_erode;
    functype                  m_dilate;
    last_op_functype          m_diff;
    e_MorphoPipelineOperation m_op;
  };
} // namespace mln::morpho
