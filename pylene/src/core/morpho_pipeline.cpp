#include <mln/core/canvas/morpho_pipeline.hpp>

namespace mln::morpho
{
  void MorphoPipeline::execute()
  {
    std::any dil;
    std::any ero;
    switch (m_op)
    {
    case e_MorphoPipelineOperation::Closing:
      dil = m_dilate(m_input);
      m_erode_out(dil, m_output);
      break;
    case e_MorphoPipelineOperation::Opening:
      ero = m_erode(m_input);
      m_dilate_out(ero, m_output);
      break;
    case e_MorphoPipelineOperation::Grad_thick:
      dil = m_dilate(m_input);
      ero = m_erode(m_input);
      m_diff(dil, ero, m_output);
      break;
    case e_MorphoPipelineOperation::Grad_ext:
      dil = m_dilate(m_input);
      m_diff(dil, m_input, m_output);
      break;
    case e_MorphoPipelineOperation::Grad_int:
      ero = m_erode(m_input);
      m_diff(m_input, ero, m_output);
      break;
    case e_MorphoPipelineOperation::Top_hat:
      ero = m_erode(m_input);
      dil = m_dilate(ero); // opening
      m_diff(dil, m_input, m_output);
      break;
    case e_MorphoPipelineOperation::Bot_hat:
      dil = m_dilate(m_input);
      ero = m_erode(dil); // closing
      m_diff(m_input, ero, m_output);
      break;
    }
  }

  /*void MorphoPipeline::execute_inplace()
  {
    std::any dil;
    std::any ero;
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
  } RECHECK might be unnecessary*/
} // namespace mln::morpho
