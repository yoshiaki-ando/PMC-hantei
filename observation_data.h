/*
 * observation_data.h
 *
 * 各緯度における、観測データ
 *
 *  Created on: 2024/07/01
 *      Author: ando
 */

#ifndef OBSERVATION_DATA_H_
#define OBSERVATION_DATA_H_

constexpr int Num_data { 101 };

class Observation_data{
public:
  Observation_data(void):
    i_alpha (-1), latitude(0.0), longitude(0.0), pmc_exist{ false }
  {
    data = new double [Num_data];
    for(int i = 0; i < Num_data; i++){
      data[i] = 0.0;
    }

  };
  ~Observation_data(void){
    delete [] data;
  };

  int i_alpha;
  double latitude, longitude;
  double *data;
  bool pmc_exist;
};



#endif /* OBSERVATION_DATA_H_ */
