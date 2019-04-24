#pragma once

#include <py/core/type_info.hpp>

#include <string>


std::string sampletypeid_to_numpystr(mln::py::Info::type_id sample_type);
mln::py::Info::type_id numpystr_to_sampletypeid(const std::string &s);
