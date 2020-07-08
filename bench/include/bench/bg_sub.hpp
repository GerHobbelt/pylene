#pragma once

#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage_fwd.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>

#include <opencv2/opencv.hpp>

#include <variant>

using pln_se_t = std::variant<mln::se::disc, mln::se::rect2d>;

void pln_bg_sub_pipe_views(const mln::image2d<mln::rgb8>& img_color, const mln::image2d<mln::rgb8>& bg_color,
                           mln::image2d<uint8_t>& output, const pln_se_t& se);


void pln_bg_sub_pipe_algos(const mln::image2d<mln::rgb8>& img_color, const mln::image2d<mln::rgb8>& bg_color,
                           mln::image2d<uint8_t>& output, const pln_se_t& se);

void cv_bg_sub_pipe(const cv::Mat& img_color, const cv::Mat& bg_color, cv::Mat& output, const cv::Mat& se);
