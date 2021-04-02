//
// Created by alexandre on 02/04/2021.
//

#include "mln/contrib/segdet/filter.hpp"


namespace mln::contrib::segdet
{
  void predict(Filter & f, uint32_t t)
  {
    f.S_predicted = A * f.S + f.W;
    f.X_predicted = C * f.S_predicted + f.N;

    f.n_min = f.X_predicted(0,0) - f.X_predicted(1, 0) / 2;
    f.n_max = f.X_predicted(0,0) + f.X_predicted(1, 0) / 2;

    f.H = A * f.H * A_transpose;

    f.W = Eigen::Matrix<double, 4, 1>(0,0,0,0);

    
    f.observation = std::nullopt;
  }
}