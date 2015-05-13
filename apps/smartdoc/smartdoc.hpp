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
constexpr float CORNER_TOLERANCE = 0.35;

// Min/Max Size of the target (in % the image size)
constexpr float MIN_OBJECT_SIZE = 0.05;
constexpr float MAX_OBJECT_SIZE = 0.3;

constexpr float WEIGHT_DEPTH = 1;
constexpr float WEIGHT_QUAD = 1;
constexpr float WEIGHT_NOISE = 1;

constexpr float MAX_DISTANCE_INTER_FRAME = 100;
constexpr int NQUAD = 7;




using tree_t = mln::morpho::component_tree<unsigned, mln::image2d<unsigned> >;

struct process_result_t
{
  std::array<mln::point2d, 4> points;
  float                       energy;
};


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
std::array<process_result_t, NQUAD>
process(tree_t& tree,
        const mln::image2d<mln::uint16>& imdepth,
        const char* csvfile = nullptr,
        const char* treefile = nullptr);


void
draw_quad_superimpose(std::array<mln::point2d, 4> quad,
                      const mln::image2d<mln::uint16>& depth,
                      mln::image2d<mln::rgb8>& out);

void
draw_quad_superimpose(std::array<mln::point2d, 4> quad,
                      mln::image2d<mln::rgb8>& out);




#endif // ! APPS_SMARTDOC_SMARTDOC_HPP
