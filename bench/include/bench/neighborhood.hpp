#pragma once

#include <mln/core/image/ndimage_fwd.hpp>

#include <cstdint>
#include <cstddef>

void Sum(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Average(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Erosion(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Isotropic_Diffusion(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);
void Anisotropic_Diffusion(const mln::image2d<uint8_t>& input, mln::image2d<uint8_t>& output);

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
