#ifndef APPS_SMARTDOC_BORDER_DETECTION_HPP
# define APPS_SMARTDOC_BORDER_DETECTION_HPP

#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include "unhomo.hpp"

/*********************************************/
/***   CONFIGURATION                       ***/
/*********************************************/

static constexpr unsigned RED_THRESHOLD = 117; //117;  //350;
static constexpr unsigned BORDER_IN = 3;
static constexpr unsigned START_END_STEP = 30;
static constexpr unsigned START_END_STEP_T = 10;


/// \brief Detect the document and return the corner points.
/// If \p debug_filename is non-NULL, it outputs the image with
/// the border drawn in blue.
pquad_t
border_detection(const mln::image2d<mln::rgb8>& f,
                 const char* debug_filename = NULL);



#endif // ! APPS_SMARTDOC_BORDER_DETECTION_HPP
