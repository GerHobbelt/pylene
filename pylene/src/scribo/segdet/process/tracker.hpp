#pragma once

#include "../descriptor.hpp"
#include "tracker_impl.hpp"

#include <memory>

namespace scribo::internal
{
  struct Tracker
  {
    std::unique_ptr<Tracker_impl> impl;

    Tracker(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~Tracker() = default;

    Tracker(Tracker&&) noexcept            = default;
    Tracker& operator=(Tracker&&) noexcept = default;

    void predict();
    void integrate(int t, const Descriptor& descriptor);

    int get_position() const noexcept;
  };
} // namespace scribo::internal