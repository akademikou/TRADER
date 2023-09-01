namespace MY {
typedef unsigned int uint;
}
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include "MATRIX.h"
#include "DEFINITIONS.h"
#include "GA_AUX.h"
#include "CLASS_KON.h"
#include "CLASS_GA.h"
std::mt19937_64 mt_A;
std::random_device rd_A;
std::uniform_real_distribution<double> dist_R;
std::uniform_int_distribution<unsigned int> dist_I;
/*******************************************************************************/
GA::~GA() {
	if (aSNR != NULL) {
		aSNR = Free_Matrix_1D(aSNR);
	}
	if (uSNR != NULL) {
		uSNR = Free_Matrix_1D(uSNR);
	}
	if (araSNR != NULL) {
		araSNR = Free_Matrix_1D(araSNR);
	}
	if (COST != NULL) {
		COST = Free_Matrix_1D(COST);
	}
	if (dPUB != NULL) {
		dPUB = Free_Matrix_2D(dPUB);
	}
}
/*******************************************************************************/
void GA::init_HAFIZA() {
	aSNR = Init_Matrix_1D<double>(nDIM);
	uSNR = Init_Matrix_1D<double>(nDIM);
	araSNR = Init_Matrix_1D<double>(nDIM);
	COST = Init_Matrix_1D<double>(nPUB);
	nCOST = Init_Matrix_1D<double>(nPUB);
	dPUB = Init_Matrix_2D<double>(nPUB, nDIM); // IKILI kodlamada MAP edilenler dahil
}
/*******************************************************************************/
GA::GA(KON *_infunc, unsigned int _nI, unsigned int _nB) {
	nProcessors = omp_get_max_threads();
	omp_set_num_threads(nProcessors);
	aSNR = NULL;
	uSNR = NULL;
	araSNR = NULL;
	COST = NULL;
	dPUB = NULL;
	mut_rate = 0.01;
	cro_rate = 1.00;
	func = _infunc;
	nDIM = func->GET_DIM();
	iterMAX = _nI;
	nB = _nB;
	nPOP = int(pow(nB, nDIM));		// herbir boyutta nb farklı değer
	nCHILD = int(nPOP * cro_rate);
	nPUB = nPOP + nCHILD;
	nElit = int(1 + nPUB * 0.01);		// elit birey sayısı topluluğun %5'u

	std::cout << "nPOP\t:" << nPOP << std::endl;
	std::cout << "nCHILD\t:" << nCHILD << std::endl;
	std::cout << "nPUB\t:" << nPUB << std::endl;
	std::cout << "nDIM\t:" << nDIM << std::endl;
	std::cout << "nElit\t:" << nElit << std::endl;
	std::cout << "________________________\n\n";

	dist_R.param(std::uniform_real_distribution<double>::param_type { 0, 1 });
	dist_I.param(
			std::uniform_int_distribution<unsigned int>::param_type { 0, nPUB
					- 1 });
	mt_A.seed(rd_A());
	mt_A.seed(std::chrono::steady_clock::now().time_since_epoch().count());

	init_HAFIZA();
	func->GET_SNR(aSNR, uSNR);
	for (unsigned int j = 0; j < nDIM; j++) {
		araSNR[j] = uSNR[j] - aSNR[j];
	}
}
/*******************************************************************************/
void GA::init_POP(bool use_gaussian) {
	bool use_balanced = true;
	std::cout << "INIT\n";
	/****************************************************************
	 for (unsigned int i = 1; i < nPUB; i++) {
	 for (unsigned int j = 0; j < nDIM; j++) {
	 VEL[i][j] = minVEL;
	 dPUB[i][j] = aSNR[j] + araSNR[j] * dist_R(mt_A);
	 }
	 }
	 */
	/****************************************************************/
	if (use_gaussian) {
		for (unsigned int j = 0; j < nDIM; j++) {
			aSNR[j] = dPUB[0][j] - araSNR[j] / 2;
			uSNR[j] = dPUB[0][j] + araSNR[j] / 2;
			araSNR[j] = uSNR[j] - aSNR[j];
		}
		for (unsigned int i = 1; i < nPOP; i++) {
			for (unsigned int j = 0; j < nDIM; j++) {
				dPUB[i][j] = dPUB[0][j] + araSNR[j] * (2 * dist_R(mt_A) - 1);
			}
		}
	}

	else if (use_balanced) {
		for (unsigned int i = 1; i < nPOP; i++) {
			for (unsigned int j = 0; j < nDIM; j++) {
				dPUB[i][j] = aSNR[j]
						+ araSNR[j] * double(int(i / pow(nB, j)) % nB)
								/ double(nB - 1.0);	// dengeli dağıtım
			}
		}
	} else {
		for (unsigned int i = 1; i < nPOP; i++) {
			for (unsigned int j = 0; j < nDIM; j++) {
				dPUB[i][j] = aSNR[j] + araSNR[j] * dist_R(mt_A);// rastlantısal dağıtım
			}
		}
	}
	TEKRAR_FULL(dPUB, nPOP, nDIM);
	unsigned int ind = 0; // en büyük değerli bireyin dizideki indisi
	double fmax = COST[0];
	double Tot_Cost = 0;
	for (unsigned int i = 0; i < nPOP; i++) {
		func->UYGUNLUK(dPUB[i], COST[i]);
	}
	for (unsigned int i = 0; i < nPOP; i++) {
		fmax = nCOST[i];
		ind = i;
		for (unsigned int j = i + 1; j < nPUB; j++) {
			if (fmax < nCOST[j]) {
				fmax = nCOST[j];
				ind = j;
			}
		}
		for (unsigned int j = 0; j < nDIM; j++) {
			TAKAS(dPUB[i][j], dPUB[ind][j]);
		}
		TAKAS(COST[i], COST[ind]);
	}
	for (unsigned int i = 0; i < nPOP; i++) {
		if (fmax < COST[i]) {
			fmax = COST[i];
		}
	}
	for (unsigned int i = 0; i < nPOP; i++) {
		nCOST[i] = fmax - COST[i];
		Tot_Cost += nCOST[i];
	}
	for (unsigned int i = 0; i < nPOP; i++) {
		nCOST[i] /= Tot_Cost;
	}	// nCOST normalize edildi (0 1] aralığına alındı
	SIRALA(nPOP, nDIM, nCOST, COST, dPUB);
}
/***********************************************************************************/
void GA::URET() {
#pragma omp parallel
	{
#pragma omp for
		for (int i = 1; i < (int) nPOP; i++) {
			int S = nPOP + i;
			for (unsigned int j = 0; j < nDIM; j++) {
				double df = araSNR[j] * 0.001;
				double cr_d = dist_R(mt_A);
				dPUB[S][j] = (cr_d) * dPUB[0][j] + (1 - cr_d) * dPUB[i][j]
						+ (dist_R(mt_A) - 0.5) * df;
				if (dist_R(mt_A) < mut_rate) {
					dPUB[S][j] = aSNR[j] + araSNR[j] * dist_R(mt_A);
				}
			}
			func->UYGUNLUK(dPUB[S], COST[S]);
			if (COST[S] < COST[i]) {
				for (unsigned int j = 0; j < nDIM; j++) {
					dPUB[i][j] = dPUB[S][j];
				}
				COST[i] = COST[S];
			}
		}
	}
	unsigned int ind = 0; // en büyük değerli bireyin dizideki indisi
	double fmin = COST[0];
	for (unsigned int i = 0; i < nPOP; i++) {
		if (fmin > COST[i]) {
			fmin = COST[i];
			ind = i;
		}
	}
	for (unsigned int j = 0; j < nDIM; j++) {
		TAKAS(dPUB[0][j], dPUB[ind][j]);
	}
	TAKAS(COST[0], COST[ind]);
}
/***********************************************************************************/
void GA::SEC() { // nPOP 50 cross 0.2- >50+10=60 nElit
	bool RANK = false;
	double fmax;

	/*	En düşük uygunluklu nElit sayısınca birey listenin başına alınıyor
	 RANK yönteminde gerek yok zaten sıralanmıştı
	 */
	unsigned int ind = 0; // en büyük değerli bireyin dizideki indisi
	for (unsigned int i = 0; i < nElit; i++) {
		fmax = nCOST[i];
		ind = i;
		for (unsigned int j = nPOP + i; j < nPUB; j++) {
			if (fmax < nCOST[j]) {
				fmax = nCOST[j];
				ind = j;
			}
		}
		for (unsigned int j = 0; j < nDIM; j++) {
			TAKAS(dPUB[i][j], dPUB[ind][j]);
		}
		TAKAS(COST[i], COST[ind]);
//		TAKAS(nCOST[i], nCOST[ind]);
	}
	return;
	fmax = COST[0];
	double Tot_Cost_1 = 0;
	double Tot_Cost_2 = 0;
	double sum, r;
	for (unsigned int i = nElit; i < nPUB; i++) {
		if (fmax < COST[i]) {
			fmax = COST[i];
		}
	}	// en büyüğün bulunması
	for (unsigned int i = nElit; i < nPUB; i++) {
		nCOST[i] = fmax - COST[i];
		Tot_Cost_1 += nCOST[i];
	}
	for (unsigned int i = nElit; i < nPUB; i++) {
		nCOST[i] /= Tot_Cost_1;
	}	// nCOST normalize edildi (0 1] aralığına alındı

	if (RANK) {
		SIRALA(nPUB, nDIM, nCOST, COST, dPUB);
		for (unsigned int i = 0; i < nPUB; i++) {
			nCOST[i] = nPUB - i;
		}	// sıra değeri cost ile değiştirildi
	}
	/* rulet tekerleği yöntemi uygulanacak */
	for (unsigned int i = nElit; i < nPUB; i++) {
		Tot_Cost_2 += nCOST[i];
	}	// ruletin maksimum sayısı belirlendi
	for (unsigned int i = nElit; i < nPOP; i++) {
		r = Tot_Cost_2 * dist_R(mt_A);
		sum = 0;
		unsigned int j = i - 1;
		while (sum < r) {
			j++;
			sum += nCOST[j];
		}
		Tot_Cost_2 -= nCOST[j];
		TAKAS(COST[i], COST[j]);
		TAKAS(nCOST[i], nCOST[j]);
		for (unsigned int k = 0; k < nDIM; k++) {
			TAKAS(dPUB[i][k], dPUB[j][k]);
		}
	}
//	TEKRAR_FULL(dPUB, nPOP,nDIM);
}
/***********************************************************************************/
bool GA::CRITERIA() {
	const double eps = 1E-20;
//	static	double old_best = 0;
	static double old_mean = 0;	// ortlama
	static double old_sd = 0;	// standart sapma
	/*	ortalama ve standart sapmanın değişmemesi arayıcıların
	 bir noktada kümelendiğini yada saplandığını gösterir.
	 */
	double mean = 0;
	double sd = 0;
//	double best = COST[0];
	for (unsigned int i = 0; i < nPOP; i++) {
		mean += COST[i];
	}
	mean /= nPOP;
	for (unsigned int i = 0; i < nPOP; i++) {
		sd += (COST[i] - mean) * (COST[i] - mean);
	}	//
	sd = sqrt(sd / nPOP);
	/* hepsi olmak zorunda değil tabii */
	if (fabs(old_mean - mean) < eps && fabs(old_sd - sd) < eps &&
//		old_best == best ||
			sd < 1E-10) {
		return true;
	}
	old_mean = mean;
	old_sd = sd;
//	old_best = best;
	return false;
}
/***********************************************************************************/
double GA::RUN(double *EN_IYI) {
	COST[0] = 1E20;
	std::cout << "running ..." << std::endl;
	bool use_gaussian = false;
	double old;
	int nTEST = 500;
	int nDEL = 50;
	func->nILK = 0;
	do {
		func->nSON = func->nILK + nTEST;
		if (func->nSON >= func->nDATA) {
			func->nSON = func->nDATA - 1;
		}
		int stage = 0;
		do {
			std::cout << "stage: " << stage << "\n";
			old = COST[0];
			init_POP(use_gaussian);
			for (unsigned int iter = 0; iter <= iterMAX; iter++) {
				URET();
				//		//	SEC();
				//		//	if (CRITERIA()) { break; std::cout << "CRITERIA "<<iter << std::endl; }
				if (iter % 100 == 0) {
					std::cout << iter << "\t" << COST[0] << std::endl;
				}
			}
			use_gaussian = true;
			stage++;
		} while (COST[0] < old || stage < 5);
		//	func->YAZ_EMIR( "GA_EMIR.txt",dPUB[0]);
		func->YAZ_OPT("GA_EMIR.txt", dPUB[0]);
		func->nILK += nDEL;
		std::cout << "____________________________\n";
		std::cout << func->nILK << "\t" << func->nSON << "\n";
	} while (func->nSON < func->nDATA - 1);

	for (unsigned int i = 0; i < nDIM; i++) {
		EN_IYI[i] = dPUB[0][i];
	}
	return COST[0];
}
/***********************************************************************************/
