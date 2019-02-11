#pragma once

#include <boost/program_options/options_description.hpp>

class AttributeArgParser
{
public:
  virtual boost::program_options::options_description& description() = 0;
};
