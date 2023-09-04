namespace MY {
typedef unsigned int uint;
}
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <omp.h>

#include "CLASS_GA.h"
#include "CLASS_PSO.h"
//#include "CLASS_PSO_DIM.h"
#include "CLASS_FLC.h"
#include "CLASS_LVQ.h"
#include "MATRIX.h"
std::mt19937_64 mt_A_EA;
std::random_device rd_A_EA;
std::uniform_real_distribution<double> dist_R_EA;
std::uniform_int_distribution<int> dist_I_EA;
#include "DEFINITIONS.h"
#include "AUX_FUNC.h"

int main() {
	std::fstream DOSYA;
	std::string DOSYA_ADI;

//	beep();
#if defined(_MSC_VER)
	system("cls");
#elif defined(__GNUC__)
	printf("\033[2J");	// clear the screen
	printf("\033[1;1H");	// move cursor home
	printf("\033[3J");	// clear the scroll back
#endif

	int nProcessors = omp_get_max_threads();
	std::cout << "num Processor: " << nProcessors << std::endl;
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout << std::setprecision(6);

	dist_R_EA.param(std::uniform_real_distribution<double>::param_type { -100,
			100 });
	mt_A_EA.seed(
			static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
	/**************************************************************************************/
	std::vector<std::vector<double>> TI_DATA;
	std::vector<int> BEST_poz;
	std::vector<int> BEST_ord;
	WORK_SPACE(TI_DATA, BEST_poz, BEST_ord);

	int nDATA = (int) TI_DATA.size();
	const int nGIR = (int) TI_DATA[0].size();
	const int nCIK = 3;
	const int nPSS = 5;
	const int nH = nCIK * nPSS;
//	for (int i = 0; i < (int)TI_DATA.size(); i++)
//	{for (int j = 0; j < (int)TI_DATA[0].size(); j++){TI_DATA[i][j]*=0.01;}}
//	LVQ_LEAN(nGIR,nCIK,nPSS,BEST_poz,BEST_ord,TI_DATA);
	//return 0;
	/*******************************************************************/
	double *IN_tmp = Init_Matrix_1D<double>(nH * nGIR);
	KON *func = new KON();
	int nDIM = func->GET_DIM();
	if (nDIM != (nH * nGIR)) {
		std::cout << "SIZE MISMATCH\n";
#if defined(_MSC_VER)
		system("pause");
#elif defined(__GNUC__)
		int sil = 0;
		std::cout << "press 1 to continue\n";
		std::cin >> sil;
#endif
	}
	double SONUC = 0;
	int nITER = 40;
//	PSO_DIM*OPTIM_PSO = new PSO_DIM(func, nITER);
	PSO *OPTIM_PSO = new PSO(func, nITER);
	SONUC = OPTIM_PSO->RUN(IN_tmp);
	std::cout << "PSO EXECUTED \n ----------------------------\n" << std::endl;
	std::cout << "# DEGER : " << SONUC << std::endl;
	std::cout << "____________________________________________\n\n"
			<< std::endl;
	/************************************************************/
	PSO_to_R(nH, nGIR, nDATA);
#if defined(_MSC_VER)
		system("pause");
#elif defined(__GNUC__)
	std::cout << "finished ...\n";
#endif
	return 0;
}

