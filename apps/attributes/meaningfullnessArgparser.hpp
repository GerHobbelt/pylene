#pragma once

#include "argparser.hpp"
#include "meaningfullness.hpp"

#include <apps/tos/topology.hpp>

#include <boost/program_options/errors.hpp>
#include <boost/program_options/variables_map.hpp>

#include <string>


class MeaningFullNessArgParser : public AttributeArgParser
{
public:
  MeaningFullNessArgParser();
  virtual boost::program_options::options_description& description();

  template <typename V, typename T>
  mln::image2d<float> run(const boost::program_options::variables_map& vm, const mln::image2d<V>& f,
                          const mln::image2d<T>& K, const mln::image2d<unsigned>& parent,
                          const std::vector<unsigned>& S);

private:
  boost::program_options::options_description m_desc;
};

/*****************************/
/** Implementation         ***/
/*****************************/

template <typename V, typename T>
mln::image2d<float> MeaningFullNessArgParser::run(const boost::program_options::variables_map& vm,
                                                  const mln::image2d<V>& f, const mln::image2d<T>& K,
                                                  const mln::image2d<unsigned>& parent, const std::vector<unsigned>& S)
{
  float a0    = vm["a0"].as<float>();
  float a1    = vm["a1"].as<float>();
  float alpha = vm["alpha"].as<float>();
  int   eps   = vm["eps"].as<int>();

  std::cout << "a0: " << a0 << std::endl;
  std::cout << "a1: " << a1 << std::endl;
  std::cout << "alpha: " << alpha << std::endl;
  std::cout << "eps: " << eps << std::endl;
  return meaningfullness(f, K, parent, S, alpha, a0, a1, eps);
}
