#pragma once

#include <opencv2/opencv.hpp>

#include "bg_sub_pln.hpp"

void cv_bg_sub_pipe(const cv::Mat& img_color, const cv::Mat& bg_color, cv::Mat& output, const cv::Mat& se);
