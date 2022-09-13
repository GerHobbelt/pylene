#pragma once

#include "../descriptor.hpp"
#include "filter_impl.hpp"

#include <memory>

namespace scribo::internal
{
  struct Filter
  {
    std::unique_ptr<Filter_impl> impl;

    Filter(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~Filter() = default;

    Filter(Filter&&) noexcept            = default;
    Filter& operator=(Filter&&) noexcept = default;

    void predict();
    void integrate(int t, const Descriptor& descriptor);

    size_t get_position();
  };
} // namespace scribo::internal