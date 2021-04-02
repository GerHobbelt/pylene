#pragma once

#include "segment.hpp"
#include <Eigen/Dense>
#include <vector>

#define SEGDET_NB_VALUES_TO_KEEP 30

#define SEGDET_VARIANCE_POSITION 2
#define SEGDET_VARIANCE_THICKNESS 1
#define SEGDET_VARIANCE_LUMINOSITY 12

namespace mln
{
  namespace contrib
  {
    namespace segdet
    {
      static const Eigen::Matrix<double, 4, 4> A((Eigen::Matrix<double, 4, 4>() << 1, 1, 0, 0,
                                                                                   0, 1, 0, 0,
                                                                                   0, 0, 1, 0,
                                                                                   0, 0, 0, 1).finished());
      static const Eigen::Matrix<double, 4, 4> A_transpose((Eigen::Matrix<double, 4, 4>() << 1, 0, 0, 0,
                                                                                              1, 1, 0, 0,
                                                                                              0, 0, 1, 0,
                                                                                              0, 0, 0, 1).finished());
      static const Eigen::Matrix<double, 3, 4> C((Eigen::Matrix<double, 3, 4>() << 1, 0, 0, 0,
                                                                                      0, 0, 1, 0,
                                                                                      0, 0, 0, 1).finished());
      static const Eigen::Matrix<double, 3, 3> Vn((Eigen::Matrix<double, 3, 3>()
                                                 << SEGDET_VARIANCE_POSITION, 0, 0,
                                                   0, SEGDET_VARIANCE_THICKNESS, 0,
                                                   0, 0, SEGDET_VARIANCE_LUMINOSITY).finished());

      struct Filter
      {
        bool is_horizontal;             // set if filter is horizontal (t = x, n = y)
        u_int32_t first;                // t value of first integration

        std::vector<uint32_t> t_values; // t values of SEGDET_NB_VALUES_TO_KEEP last integrations
        std::vector<uint32_t> n_values;

        uint nb_current_slopes_over_slope_max;
        double slope_max;
        double first_slope;
        std::vector<double> thicknesses;
        std::vector<double> luminosities;
        std::vector<double> slopes;
        std::vector<Point> under_other; //
        std::vector<Point> currently_under_other;
        std::vector<Point> segment_points;

        Eigen::Matrix<double, 4, 1> S;  // state matrix {{position (n)}, {slope}, {thickness}, {luminosity}}
        Eigen::Matrix<double, 4, 1> W;  // noise matrix
        Eigen::Matrix<double, 3, 1> N;  // measured noise matrix
        Eigen::Matrix<double, 4, 4> H;  // S prediction error variance matrix

        u_int32_t n_min;
        u_int32_t n_max;
        double sigma_position;
        double sigma_thickness;
        double sigma_luminosity;

        Eigen::Matrix<double, 4, 1> S_predicted;
        Eigen::Matrix<double, 3, 1> X_predicted;
        std::optional<Eigen::Matrix<double, 3, 1>> observation;    // matrix {{position (n)},{thickness},{luminosity}}, nullopt if none was matched
        u_int32_t observation_position; // t value referring to the position of the last observation
        u_int32_t observation_distance; // n distance from last observation to current prediction

        u_int32_t last_integration;     // t value referring to the position of the last integration
      };

      void predict(Filter & f, uint32_t t);

    }
  }
}
