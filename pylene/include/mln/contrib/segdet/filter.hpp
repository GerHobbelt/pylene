#pragma once

#include "segment.hpp"
#include <Eigen/Dense>
#include <utility>
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
      struct Observation
      {
        Observation(Eigen::Matrix<double, 3, 1>  obs, uint32_t matchCount, uint32_t t)
            : obs(std::move(obs))
            , match_count(matchCount)
            , t(t)
        {}
        Eigen::Matrix<double, 3, 1> obs;
        uint32_t match_count;
        uint32_t t;
      };

      static const Eigen::Matrix<double, 4, 4>
          A((Eigen::Matrix<double, 4, 4>() << 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
      static const Eigen::Matrix<double, 4, 4>
          A_transpose((Eigen::Matrix<double, 4, 4>() << 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
      static const Eigen::Matrix<double, 3, 4>
          C((Eigen::Matrix<double, 3, 4>() << 1, 0, 0, 0,
                                              0, 0, 1, 0,
                                              0, 0, 0, 1).finished());
      static const Eigen::Matrix<double, 4, 3>
          C_transpose((Eigen::Matrix<double, 4, 3>() << 1, 0, 0,
                                                           0, 0, 0,
                                                           0, 1, 0,
                                                           0, 0, 1).finished());
      static const Eigen::Matrix<double, 3, 3> Vn((Eigen::Matrix<double, 3, 3>() << SEGDET_VARIANCE_POSITION, 0, 0, 0,
                                                   SEGDET_VARIANCE_THICKNESS, 0, 0, 0, SEGDET_VARIANCE_LUMINOSITY)
                                                      .finished());

      struct Filter
      {
        Filter(bool isHorizontal, uint32_t t_integration, double slopeMax, Eigen::Matrix<double, 3, 1> observation)
          : is_horizontal(isHorizontal)
          , slope_max(slopeMax)
          , S((Eigen::Matrix<double, 4, 1>() << observation(0, 0), 0, observation(1, 0), observation(2, 0)).finished())
          , W(Eigen::Matrix<double, 4, 1>::Zero())
          , H(Eigen::Matrix<double, 4, 4>::Identity())
          , observation(std::nullopt)
          , observation_distance(0)
          , last_integration(t_integration)
        {
          first    = t_integration;
          t_values = std::vector<uint32_t>({t_integration});
          n_values = std::vector<uint32_t>({static_cast<uint32_t>(observation(0,0))});
          slopes = std::vector<double>({0});
          thicknesses = std::vector<double>({observation(1,0)});
          luminosities = std::vector<double>({observation(2,0)});
          nb_current_slopes_over_slope_max = 0;
          first_slope = std::nullopt;
        }
        bool      is_horizontal; // set if filter is horizontal (t = x, n = y)
        u_int32_t first;         // t value of first integration

        std::vector<uint32_t> t_values; // t values of SEGDET_NB_VALUES_TO_KEEP last integrations
        std::vector<uint32_t> n_values;

        uint                nb_current_slopes_over_slope_max{};
        double              slope_max;
        std::optional<double>              first_slope{};
        std::vector<double> thicknesses;
        std::vector<double> luminosities;
        std::vector<double> slopes;
        std::vector<Point>  under_other; //
        std::vector<Point>  currently_under_other;
        std::vector<Point>  segment_points;

        Eigen::Matrix<double, 4, 1> S; // state matrix {{position (n)}, {slope}, {thickness}, {luminosity}}
        Eigen::Matrix<double, 4, 1> W; // noise matrix
        Eigen::Matrix<double, 3, 1> N; // measured noise matrix
        Eigen::Matrix<double, 4, 4> H; // S prediction error variance matrix

        u_int32_t n_min{};
        u_int32_t n_max{};
        double    sigma_position{};
        double    sigma_thickness{};
        double    sigma_luminosity{};

        Eigen::Matrix<double, 4, 1> S_predicted;
        Eigen::Matrix<double, 3, 1> X_predicted;
        std::optional<Observation>
                  observation;          // matrix {{position (n)},{thickness},{luminosity}}, nullopt if none was matched
        u_int32_t observation_distance; // n distance from last observation to current prediction

        u_int32_t last_integration; // t value referring to the position of the last integration
      };

      void        predict(Filter& f);
      bool accepts(const Filter& filter, const Eigen::Matrix<double, 3, 1>& obs, uint32_t min, uint32_t max);
      std::optional<Observation> choose_nearest(Filter& f, Observation& obs);
      void integrate(Filter& f, uint32_t t);


    } // namespace segdet
  }   // namespace contrib
} // namespace mln
