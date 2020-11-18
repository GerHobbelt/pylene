#pragma once

#include <functional>

#include <mln/core/algorithm/clone.hpp>
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

  enum e_MorphoPipelineOperation
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
  template <typename SE>
  class MorphoPipeline
  {
  public:
    template <class Image>
    MorphoPipeline(const e_MorphoPipelineOperation op, Image& input, const SE& se)
      : m_input{input}
      , m_se{se}
    {
      typedef Image (*functype)(Image&, const SE&);
      functype erode  = mln::morpho::parallel::erosion;
      functype dilate = mln::morpho::parallel::dilation;
      functype id     = mln::morpho::details::identity;

      m_end = details::e_morphoFinish::None;
      switch (op)
      {
      case e_MorphoPipelineOperation::Closing:
        m_funcs[0] = dilate;
        m_funcs[1] = erode;
        break;
      case e_MorphoPipelineOperation::Opening:
        m_funcs[0] = erode;
        m_funcs[1] = dilate;
        break;
      case e_MorphoPipelineOperation::Grad_thick:
        m_funcs[0] = dilate;
        m_funcs[1] = erode;
        m_end      = details::e_morphoFinish::Subtraction;
        break;
      case e_MorphoPipelineOperation::Grad_ext:
        m_funcs[0] = dilate;
        m_funcs[1] = id;
        m_end      = details::e_morphoFinish::Subtraction;
        break;
      case e_MorphoPipelineOperation::Grad_int:
        m_funcs[0] = id;
        m_funcs[1] = erode;
        m_end      = details::e_morphoFinish::Subtraction;
        break;
      case e_MorphoPipelineOperation::Top_hat:
        // FIXME opening(input) - id(input)
        break;
      case e_MorphoPipelineOperation::Bot_hat:
        // FIXME id(input) - closing(input)
        break;
      default:
        m_funcs[0] = id;
        m_funcs[1] = id;
        break;
      }
    }

    ndbuffer_image execute()
    {
      ndbuffer_image res;
      switch (m_end)
      {
      case details::e_morphoFinish::None: {
        /* Then inputs for op1 and op2 are the same image: res = op2(op1(m_input))*/
        auto output1 = m_funcs[0](m_input, m_se);
        res          = m_funcs[1](output1, m_se);
        break;
      }
      case details::e_morphoFinish::Subtraction: {
        /* Then inputs for op1 and op2 are both the starting image: res = op1(m_input) - op2(m_input)*/
        using namespace mln::view::ops;
        auto output1 = m_funcs[0](m_input, m_se);
        auto output2 = m_funcs[1](m_input, m_se);
        res = mln::clone(output1 - output2);
        break;
      }
      default:
        throw std::runtime_error("Unrecognized ending for morphological operation pipeline.");
      }
      return res;
    }

  private:
    ndbuffer_image                                            m_input;
    SE                                                        m_se;
    std::function<ndbuffer_image(ndbuffer_image&, const SE&)> m_funcs[2];
    details::e_morphoFinish                                   m_end;
  };
} // namespace mln::morpho