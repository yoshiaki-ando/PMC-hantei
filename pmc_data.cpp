/*
 * pmc_data.cpp : 観測データを読み込む
 *
 *  Created on: 2024/07/01
 *      Author: ando
 */
#include <iostream>
#include <fstream>
#include <string>

#include "observation_data.h"
#include "hantei.h"

void read_pmcdata(Observation_data *pmc_data, std::string filename){

  std::ifstream ifs_pmcdata( filename );

  /* データ読み込み */
  std::string garbage;
  ifs_pmcdata >> garbage; /* height */

  ifs_pmcdata >> garbage; /* lat */
  ifs_pmcdata >> garbage; /* lon */

  for(int i = 0; i < Num_data; i++){
    ifs_pmcdata >> garbage; /* 0 - 100 */
  }

  for(int i_alpha = 0; i_alpha < Num_latitude; i_alpha++){
    pmc_data[i_alpha].i_alpha = i_alpha;
    ifs_pmcdata >> pmc_data[i_alpha].latitude >> pmc_data[i_alpha].longitude;

    for(int k_alt = 0; k_alt < Num_data; k_alt++){
      ifs_pmcdata >> pmc_data[i_alpha].data[k_alt];
    }
  }

  ifs_pmcdata.close();

}

