/*
 * err_func.cpp
 *
 *  Created on: 2024/07/01
 *      Author: ando
 */
#include <iostream>
#include <cmath>
#include <vector>

#include <nlopt.hpp>

#include "hantei.h"

extern int iteration;

double offset(const int k_Altitude, double *obs_data){
  double slope
  = (obs_data[Upper_offset] - obs_data[Lower_offset]) / (Upper_offset - Lower_offset);
  return slope * (k_Altitude - Lower_offset) + obs_data[Lower_offset];
}

double err_func(const std::vector <double> &x, std::vector <double> &grad, void *data){
  double sq_err { 0.0 };
  double *obs = (double*)data;

  for(int n = 0; n < Num_PMC_layer; n++){
    /* n -> 高度 n + Lower_PMC -> idx: n + Lower_PMC - Lower_offset */
//    double offset = (obs[Upper_offset] - obs[Lower_offset]) / (Upper_offset - Lower_offset)
//        * (n + Lower_PMC - Lower_offset) + obs[Lower_offset];
    double err = std::max(obs[n+Lower_PMC] - offset(n+Lower_PMC, obs), 0.0)
        - x[2] * std::exp( - (n - x[0])*(n - x[0]) / 2.0 / x[1] / x[1] );
    sq_err += err*err;
  }

  iteration++;
  if ( iteration > 10000 ){
    throw nlopt::forced_stop();
  }

  return sq_err / Num_PMC_layer;
}
