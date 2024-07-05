/*
 * main.cpp
 *
 *  Created on: 2024/07/01
 *      Author: ando
 */
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <filesystem>

#include <nlopt.hpp>

#include <memory_allocate.h>

#include "hantei.h"
#include "observation_data.h"

int iteration; /* 最適化の反復数 */

std::string output_data_dir( "data" );
std::string input_data_dir( "/home/ando/98-Local/PMC_data/convert_data/" );

int main(int argc, char **argv){

  /* Usage */
  std::string filename(input_data_dir + "h08_b01_s01s02_20");
  if (argc < 2){
    std::cerr << "Filename is not specified." << std::endl;
    exit(1);
  } else {
    filename += std::string(argv[1]) + "_" + argv[2] + "00.txt";
    output_data_dir += std::string("_") + argv[1] + "_" + argv[2] + "/";
  }

  std::filesystem::create_directory(output_data_dir);

  Observation_data *pmc_data = new Observation_data [Num_latitude];

  read_pmcdata(pmc_data, filename); /* 観測データ読み込み */

  /* 二つの最適化法で判定する
   * 1: NELDERMEAD
   * 2: CRS2_LM
   */
  std::vector <double> pre_opt1(Num_optimized);
  std::vector <double> pre_opt2(Num_optimized);
  pre_opt1[0] = (Upper_PMC - Lower_PMC) / 2.0; /* 平均 */
  pre_opt1[1] = (Upper_PMC - Lower_PMC) / 4.0; /* SD */
  pre_opt1[2] = 1.0; /* Amplitude */
  pre_opt2[0] = (Upper_PMC - Lower_PMC) / 2.0; /* 平均 */
  pre_opt2[1] = (Upper_PMC - Lower_PMC) / 4.0; /* SD */
  pre_opt2[2] = 1.0; /* Amplitude */

  std::ofstream ofs_opt1(output_data_dir + "opt1.dat");
  std::ofstream ofs_opt2(output_data_dir + "opt2.dat");

  int num_pmc_occur { 0 };
  std::vector <Observation_data*> occuring_pmc; /* PMCが発生していると判定された緯度のデータを保存 */

  for(int i_alpha = 0; i_alpha < Num_latitude; i_alpha++){
    std::cout << i_alpha << std::endl;

    /* 最適化の設定 */
    std::vector <double> Optimized_param1(Num_optimized);
    std::vector <double> Optimized_param2(Num_optimized);
    /* 初期値 */
    std::copy(pre_opt1.begin(), pre_opt1.end(), Optimized_param1.begin());
    std::copy(pre_opt2.begin(), pre_opt2.end(), Optimized_param2.begin());

    /* パラメタの上限・下限            中心位置               SD                           振幅 */
    std::vector <double> Upper_Bound { Upper_PMC - Lower_PMC, (Upper_PMC - Lower_PMC)*4.0, 100.0};
    std::vector <double> Lower_Bound { 0.0,                   0.5,                         0.1 };

    std::vector <double> grad(Num_optimized); /* 目的関数のパラメタ勾配→使わない */

    nlopt::opt opt1( nlopt::LN_NELDERMEAD, Num_optimized );
    nlopt::opt opt2( nlopt::GN_CRS2_LM,    Num_optimized );

    opt1.set_min_objective( err_func, (void*)( pmc_data[i_alpha].data ) );
    opt2.set_min_objective( err_func, (void*)( pmc_data[i_alpha].data ) );
    opt1.set_xtol_rel( 1e-2 );
    opt2.set_xtol_rel( 1e-2 );
    opt1.set_lower_bounds(Lower_Bound);
    opt2.set_lower_bounds(Lower_Bound);
    opt1.set_upper_bounds(Upper_Bound);
    opt2.set_upper_bounds(Upper_Bound);

    double minf1, minf2;

    try {
      iteration = 0;
      nlopt::result result = opt1.optimize( Optimized_param1, minf1 );
//      std::cout << "(1) " << iteration << std::endl;
    } catch (std::exception &excpt){
      std::cout << "Optimization (1) failed. : " << excpt.what() << std::endl;
    }

    try {
      iteration = 0;
      nlopt::result result = opt2.optimize( Optimized_param2, minf2 );
//      std::cout << "(2) " << iteration << std::endl;
    } catch (std::exception &excpt){
      std::cout << "Optimization (2) failed. : " << excpt.what() << std::endl;
    }

    /* 最適化結果の出力 */
    ofs_opt1 << i_alpha << " "
        << Optimized_param1[0] << " "
        << Optimized_param1[1] << " "
        << Optimized_param1[2] << " "
        << minf1 << " "
        << std::endl;
    ofs_opt2 << i_alpha << " "
        << Optimized_param2[0] << " "
        << Optimized_param2[1] << " "
        << Optimized_param2[2] << " "
        << minf2 << " "
        << std::endl;
    /* 1: NELDERMEAD は、前の検索結果を初期値とせず、固定値から始める */
//    std::copy(Optimized_param1.begin(), Optimized_param1.end(), pre_opt1.begin()); /* 次の初期値に利用する */
    std::copy(Optimized_param2.begin(), Optimized_param2.end(), pre_opt2.begin()); /* 次の初期値に利用する */

    /* 確認のための、光強度の高度プロファイルのプロット */
    std::ofstream ofs_output1(output_data_dir + "output1_" + std::to_string(i_alpha) + ".dat");
    std::ofstream ofs_output2(output_data_dir + "output2_" + std::to_string(i_alpha) + ".dat");

    for(int k_alt = 0; k_alt < Num_data; k_alt++){
      ofs_output1 << k_alt << " "
                 << pmc_data[i_alpha].data[k_alt] << " ";
      ofs_output2 << k_alt << " "
                 << pmc_data[i_alpha].data[k_alt] << " ";
      if ( (k_alt >= Lower_PMC) && (k_alt <= Upper_PMC) ){
        double x = k_alt - Lower_PMC;

        ofs_output1 <<
          offset( k_alt, pmc_data[i_alpha].data )
          + Optimized_param1[2] * exp( - (x - Optimized_param1[0])*(x - Optimized_param1[0]) / 2.0 /
                                      Optimized_param1[1] / Optimized_param1[1] );
        ofs_output2 <<
          offset( k_alt, pmc_data[i_alpha].data )
          + Optimized_param2[2] * exp( - (x - Optimized_param2[0])*(x - Optimized_param2[0]) / 2.0 /
                                      Optimized_param2[1] / Optimized_param2[1] );
//        ofs_output << " "
//            << ( pmc_data[i_alpha].data[Upper_PMC] - pmc_data[i_alpha].data[70] ) / (Upper_PMC - 70 + 1) * (k_alt - 70)
//            + pmc_data[i_alpha].data[70];
      }
      ofs_output1 << "\n";
      ofs_output2 << "\n";

    }

    ofs_output1.close();
    ofs_output2.close();


    /* PMC存在判定
     *
     * どちらかの最適化アルゴリズムで、PMCの強度振幅が 0.5 を超えていれば存在していると判断する
     */
    pmc_data[i_alpha].pmc_exist = (Optimized_param1[2] > 0.5) || (Optimized_param2[2] > 0.5);
    if ( pmc_data[i_alpha].pmc_exist ){
      num_pmc_occur++;
      occuring_pmc.push_back(pmc_data + i_alpha);
    }

  }

  ofs_opt1.close();
  ofs_opt2.close();

  /*****************************************************************
   *  最適化でPMCパラメタを出力するプログラムの実行スクリプト作成  *
   *****************************************************************/

  /* 1時刻全部を1コアで実施する */
  std::ofstream ofs_script( std::string("script_") + argv[1] + "_" + argv[2] + ".csh");
  std::cout << num_pmc_occur << std::endl;
  for(int j = 0; j < num_pmc_occur; j++){
    std::cout << " j = " << j << std::endl;
    ofs_script << "./main "
        << occuring_pmc[j]->latitude << " "
        << occuring_pmc[j]->longitude << " "
        << argv[1] << " "
        << argv[2] << " "
        << occuring_pmc[j]->i_alpha << " "
        << "; ";
  }
  ofs_script << "echo " << filename << " | mail -s calc y-ando@uec.ac.jp &" << std::endl;

  /* 以下は一時刻をCPU数で分割する方法 */
//  const int Assigned_number { num_pmc_occur / Number_of_CPU };
//  const int Remainder { num_pmc_occur % Number_of_CPU };
//
//  std::cout << num_pmc_occur << " PMCs occur.\n";
//  std::ofstream ofs_script("script.csh");
//  for(int i_cpu = 0; i_cpu < Number_of_CPU; i_cpu++){
//    int j_start, j_end;
//    if ( i_cpu < Remainder ){
//      j_start = i_cpu * (Assigned_number + 1);
//      j_end = (i_cpu + 1) * (Assigned_number + 1);
//    } else {
//      j_start = i_cpu * Assigned_number + Remainder;
//      j_end = (i_cpu + 1) * Assigned_number + Remainder;
//    }
//    for(int j = j_start; j < j_end; j++){
//      ofs_script << "./main "
//          << occuring_pmc[j]->latitude << " "
//          << occuring_pmc[j]->longitude << " "
//          << argv[2] << " "
//          << argv[3] << " "
//          << occuring_pmc[j]->i_alpha << " "
//          << "; ";
//    }
//    ofs_script << " & " << std::endl;
//  }
  /* 以下は一時刻をCPU数で分割する方法（ここまで） */

  ofs_script.close();

  delete [] pmc_data;
  return 0;
}

