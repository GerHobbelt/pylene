#ifndef APPS_SMARTDOC_EXPORT_HPP
# define APPS_SMARTDOC_EXPORT_HPP

# include <mln/core/point.hpp>
# include <array>

static const char* APP_NAME = "MILENA";
static const char* APP_VERSION = "2.0";

struct SmartDoc_t
{
  bool                          good;
  std::array<mln::point2d, 4>   quad;
};

void
export_xml(const char* filename,
           const char* document_id,
           const SmartDoc_t* p_array,
           int p_array_size);


#endif // ! APPS_SMARTDOC_EXPORT_HPP
