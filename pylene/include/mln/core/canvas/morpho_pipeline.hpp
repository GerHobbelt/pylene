#pragma once

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

    enum e_morphoFunc
    {
      Erosion,
      Dilation,
      Id
    };

    enum e_morphoFinish
    {
      Subtraction,
      None
    };
  } // namespace details

  template <class Image, class SE>
  class MorphoPipeline
  /*
    TODO
    top hat,
    bot hat,
    remove unnecessary call to Tilewriting when could TileExec1 -> TileExec2 and then TileWrite the resulting, final
    tile Finish POC with sequential execution
  */
  {
  public:
    MorphoPipeline(details::e_MorphoPipelineOperation op, Image input, SE se)
      : m_input{input}
      , m_operation{op}
      , m_se{se}
    {
    }

    Image execute()
    {
      details::e_morphoFunc   funcs[2] = {details::e_morphoFunc::Id, details::e_morphoFunc::Id};
      details::e_morphoFinish end      = details::e_morphoFinish::None;
      switch (m_operation)
      {
      case details::e_MorphoPipelineOperation::Closing:
        funcs[0] = details::e_morphoFunc::Dilation;
        funcs[1] = details::e_morphoFunc::Erosion;
        break;
      case details::e_MorphoPipelineOperation::Opening:
        funcs[0] = details::e_morphoFunc::Erosion;
        funcs[1] = details::e_morphoFunc::Dilation;
        break;
      case details::e_MorphoPipelineOperation::Grad_thick:
        funcs[0] = details::e_morphoFunc::Dilation;
        funcs[1] = details::e_morphoFunc::Erosion;
        end      = details::e_morphoFinish::Subtraction;
        break;
      case details::e_MorphoPipelineOperation::Grad_ext:
        funcs[0] = details::e_morphoFunc::Dilation;
        funcs[1] = details::e_morphoFunc::Id;
        end      = details::e_morphoFinish::Subtraction;
        break;
      case details::e_MorphoPipelineOperation::Grad_int:
        funcs[0] = details::e_morphoFunc::Id;
        funcs[1] = details::e_morphoFunc::Erosion;
        end      = details::e_morphoFinish::Subtraction;
        break;
      case details::e_MorphoPipelineOperation::Top_hat:
        // FIXME opening(input) - id(input)
        break;
      case details::e_MorphoPipelineOperation::Bot_hat:
        // FIXME id(input) - closing(input)
        break;
      default:
        break;
      }

      Image res;
      switch (end)
      {
      case details::e_morphoFinish::None: {
        /* Then inputs for op1 and op2 are the same image: res = op2(op1(m_input))*/
        auto output1 = Operation1(funcs[0], m_input);
        auto output2 = Operation2(funcs[1], output1);
        res          = output2;
        break;
      }
      case details::e_morphoFinish::Subtraction: {
        /* Then inputs for op1 and op2 are both the starting image: res = op1(m_input) - op2(m_input)*/
        using namespace mln::view::ops;
        auto output1 = Operation1(funcs[0], m_input);
        auto output2 = Operation2(funcs[1], m_input);
        res = mln::clone(output1 - output2);
        break;
      }
      default:
        throw std::runtime_error("Unrecognized ending for morphological operation pipeline.");
      }
      return res;
    }

  private:
    Image                              m_input;
    details::e_MorphoPipelineOperation m_operation;
    SE                                 m_se;


    template <typename V>
    image2d<V> Operation1(details::e_morphoFunc f, image2d<V> input)
    {
      image2d<V> res = m_input;
      switch (f)
      {
      case details::e_morphoFunc::Dilation:
        res = mln::morpho::parallel::dilation(input, m_se);
        break;
      case details::e_morphoFunc::Erosion:
        res = mln::morpho::parallel::erosion(input, m_se);
        break;
      case details::e_morphoFunc::Id:
        break;
      }
      return res;
    }

    template <typename V>
    image2d<V> Operation2(details::e_morphoFunc f, image2d<V> input)
    {
      image2d<V> res = m_input;
      switch (f)
      {
      case details::e_morphoFunc::Dilation:
        res = mln::morpho::parallel::dilation(input, m_se);
        break;
      case details::e_morphoFunc::Erosion:
        res = mln::morpho::parallel::erosion(input, m_se);
        break;
      case details::e_morphoFunc::Id:
        break;
      }
      return res;
    }
  };
} // namespace mln::morpho