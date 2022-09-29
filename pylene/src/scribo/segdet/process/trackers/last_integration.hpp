#pragma once

#include "../filter_impl.hpp"

namespace scribo::internal
{
  struct LastIntegration final : public Filter_impl
  {
    LastIntegration(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~LastIntegration() override = default;

    void predict() override;
    void integrate(int t, const Descriptor& descriptor) override;
  };

} // namespace scribo::internal