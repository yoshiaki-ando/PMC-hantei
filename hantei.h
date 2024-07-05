/*
 * hantei.h
 *
 *  Created on: 2024/07/01
 *      Author: ando
 */

#ifndef HANTEI_H_
#define HANTEI_H_

#include <vector>
#include "observation_data.h"

constexpr int Number_of_CPU { 16 };

constexpr int Num_latitude { 88 };

constexpr int Lower_PMC { 78 };
constexpr int Upper_PMC { 88 };
constexpr int Num_PMC_layer { Upper_PMC - Lower_PMC + 1 };

constexpr int Upper_offset { Upper_PMC };
constexpr int Lower_offset { 70 };


constexpr int Num_optimized { 3 };

double err_func(const std::vector <double> &x, std::vector <double> &grad, void *data);
void read_pmcdata(Observation_data *pmc_data, std::string filename);
double offset(const int k_Altitude, double *obs_data);

#endif /* HANTEI_H_ */
