#pragma once

#include <mln/core/colors.hpp>

mln::rgb8 regions_lut(int x);
mln::rgb8 heat_lut(float x);

/******************************************/
/****          Implementation          ****/
/******************************************/

extern const mln::rgb8   regions_table[];
extern const std::size_t regions_table_size;



