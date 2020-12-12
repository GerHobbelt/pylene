#include <mln/core/canvas/morpho_pipeline.hpp>

namespace mln::morpho
{
  ndbuffer_image MorphoPipeline::execute() const
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
      ero = m_erode(m_input);
      dil = m_dilate(ero); // opening
      return m_diff(dil, m_input);
    case e_MorphoPipelineOperation::Bot_hat:
      dil = m_dilate(m_input);
      ero = m_erode(dil); // closing
      return m_diff(m_input, ero);
    }
    __builtin_unreachable();
  }

  void MorphoPipeline::execute_inplace()
  {
    mln::ndbuffer_image dil;
    mln::ndbuffer_image ero;
    switch (m_op)
    {
    case e_MorphoPipelineOperation::Closing:
      dil = m_dilate(m_input);
      m_output = m_erode(dil);
      break;
    case e_MorphoPipelineOperation::Opening:
      ero = m_erode(m_input);
      m_output = m_dilate(ero);
      break;
    case e_MorphoPipelineOperation::Grad_thick:
      dil = m_dilate(m_input);
      ero = m_erode(m_input);
      m_output = m_diff(dil, ero);
      break;
    case e_MorphoPipelineOperation::Grad_ext:
      dil = m_dilate(m_input);
      m_output = m_diff(dil, m_input);
      break;
    case e_MorphoPipelineOperation::Grad_int:
      ero = m_erode(m_input);
      m_output = m_diff(m_input, ero);
      break;
    case e_MorphoPipelineOperation::Top_hat:
      ero = m_erode(m_input);
      dil = m_dilate(ero); // opening
      m_output = m_diff(dil, m_input);
      break;
    case e_MorphoPipelineOperation::Bot_hat:
      dil = m_dilate(m_input);
      ero = m_erode(dil); // closing
      m_output = m_diff(m_input, ero);
      break;
    }
  }
} // namespace mln::morpho