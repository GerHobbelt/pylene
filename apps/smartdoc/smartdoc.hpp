#ifndef APPS_SMARTDOC_SMARTDOC_HPP
# define APPS_SMARTDOC_SMARTDOC_HPP

#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/colors.hpp>
#include <mln/morpho/component_tree/component_tree.hpp>


/**************************************/
/***  CONFIGURATIONS                ***/
/**************************************/

constexpr unsigned GRAINSIZE = 10;
constexpr float CORNER_TOLERANCE = 0.2;

// Min/Max Size of the target (in % the image size)
constexpr float MIN_OBJECT_SIZE = 0.05;
constexpr float MAX_OBJECT_SIZE = 0.3;

constexpr float WEIGHT_DEPTH = 1;
constexpr float WEIGHT_QUAD = 1;
constexpr float WEIGHT_NOISE = 1;





using tree_t = mln::morpho::component_tree<unsigned, mln::image2d<unsigned> >;


/// \brief Document detection method entry point.
/// It returns the pair \p (detected, QUAD) where \p detected
/// is a boolean set to true if a detection occured and \p QUAD
/// is the quadruplet of points (UL, UR, LR, LL)
///
/// \param tree The Color ToS
/// \param imdepth The depth image
/// \param[out] feedback The image with the detected object colorized (must be the same size of imdepth)
/// \param csvfile (optional) Path to the CSV file where attributes will be saved
/// \param treefile (optional) Path to the file where the tree will be saved (after filtering)
std::pair<bool, std::array<mln::point2d, 4> >
process(tree_t& tree,
        const mln::image2d<mln::uint16>& imdepth,
        mln::image2d<mln::rgb8>* feedback = nullptr,
        const char* csvfile = nullptr,
        const char* treefile = nullptr);


#endif // ! APPS_SMARTDOC_SMARTDOC_HPP
