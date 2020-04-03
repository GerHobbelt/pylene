#pragma once

#include <mln/core/image/experimental/ndimage_fwd.hpp>
#include <cstdint>

void Sum(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);
void Average(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);
void Erosion(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);
void Isotropic_Diffusion(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);
void Anisotropic_Diffusion(const mln::experimental::image2d<uint8_t>& input, mln::experimental::image2d<uint8_t>& output);

void Sum_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
           std::ptrdiff_t stride);
void Average_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
               std::ptrdiff_t stride);
void Erosion_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
               std::ptrdiff_t stride);
void Isotropic_Diffusion_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width, int height,
                           std::ptrdiff_t stride);
void Anisotropic_Diffusion_C(const uint8_t* __restrict ibuffer, uint8_t* __restrict obuffer, int width,
                             int height, std::ptrdiff_t stride);
