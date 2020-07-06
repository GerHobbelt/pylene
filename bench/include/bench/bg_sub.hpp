#pragma once

#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage_fwd.hpp>

#include <opencv2/opencv.hpp>


void pln_bg_sub_pipe_views(const mln::image2d<mln::rgb8>& img_color, const mln::image2d<mln::rgb8>& bg_color,
                           mln::image2d<uint8_t>& output, std::size_t radius);


void pln_bg_sub_pipe_algos(const mln::image2d<mln::rgb8>& img_color, const mln::image2d<mln::rgb8>& bg_color,
                           mln::image2d<uint8_t>& output, std::size_t radius);

void cv_bg_sub_pipe(const cv::Mat& img_color, const cv::Mat& bg_color, cv::Mat& output, std::size_t radius);
