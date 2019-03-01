#pragma once

#include <mln/core/image/image2d.hpp>


void Sum(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Average(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Erosion(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Isotropic_Diffusion(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Anisotropic_Diffusion(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);

void Sum_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Average_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Erosion_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Isotropic_Diffusion_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);
void Anisotropic_Diffusion_New(const mln::image2d<mln::uint8>& input, mln::image2d<mln::uint8>& output);

void Sum_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
           std::ptrdiff_t stride);
void Average_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
               std::ptrdiff_t stride);
void Erosion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
               std::ptrdiff_t stride);
void Isotropic_Diffusion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width, int height,
                           std::ptrdiff_t stride);
void Anisotropic_Diffusion_C(const mln::uint8* __restrict ibuffer, mln::uint8* __restrict obuffer, int width,
                             int height, std::ptrdiff_t stride);
