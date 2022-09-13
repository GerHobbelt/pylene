# pragma once

#include <vector>
#include <Eigen/Dense>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/ndimage_fwd.hpp>

#include "../../descriptor.hpp"

namespace scribo::internal
{
    std::vector<Eigen::Matrix<float, 3, 1>> extract_observations(const mln::image2d<uint8_t>& image, int t, int n_max,
                                                               const Descriptor& descriptor);
}