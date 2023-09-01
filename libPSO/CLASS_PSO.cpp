namespace MY {
typedef unsigned int uint;
}
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include "MATRIX.h"
#include "DEFINITIONS.h"
#include "PSO_AUX.h"
#include "CLASS_KON.h"
#include "CLASS_PSO.h"

#define YESIL	1
#define KIRMIZI 2
constexpr const MY::uint _PSO = 1;
constexpr const MY::uint _RAO = 3; // 3
constexpr const MY::uint _GA = 1;
constexpr const MY::uint _GRD = 0;
const double F = 4.5; //c1+c2+c3;
const double c1 = 0.7;
const double c2 = 1.6;			// G_BEST iptal edilmek istenirse c2=sb. c3=0;
const double c3 = (F - (c1 + c2));// L_BEST iptal edilmek istenirse c2=0   c3=sb;
// 1>W>(c1+c2+c3)/3-1>0   0.72984
const double K = 2 / fabs(2 - F - sqrt(F * (F - 4))); // fi>4 olmalı
/*******************************************************************************/
PSO::PSO(KON *_infunc, const MY::uint &_nI) {
	std::cout << c1 << "\t" << c2 << "\t" << c3 << "\t" << F << "\t" << K
			<< "\n";
	std::cout << c1 * K << "\t" << c2 * K << "\t" << c3 * K << "\n";
	nProcessors = omp_get_max_threads();
	omp_set_num_threads(nProcessors);
	VEL = NULL;
	dPUB = NULL;
	P_BEST = NULL;
	L_BEST = NULL;

	mut_rate = 0.05;
	cro_rate = 0.30;
	func = _infunc;
	iterMAX = _nI;
	nDIM = func->GET_DIM();
	// 285  /3->95 GRUP /15->19 HUCRE
	LDIM = nDIM;	// 270  /3->90 GRUP /15->18 HUCRE /45->6
	nPUB = 2 * LDIM;
	nPUB -= nPUB % nProcessors;
	nLOC = (MY::uint) (MY::uint(nPUB / nProcessors) / 3);
	nCRITER = 3;
	nTAM = nDIM + nCRITER;
	std::cout << "________________________\n\n";
	std::cout << "nDIM\t:" << nDIM << std::endl;
	std::cout << "nPUB\t:" << nPUB << std::endl;
	std::cout << "nLOC\t:" << nLOC << std::endl;
	std::cout << "iterMAX\t:" << iterMAX << std::endl;
	std::cout << "________________________\n\n";
	init_HAFIZA();
	std::uint64_t AA =
			static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	dist_I_PSO = std::uniform_int_distribution<int>(0, (int) nPUB - 1);
	for (MY::uint id = 0; id < nProcessors; id++) {
		mt_A_PSO[id] = Xoshiro256PlusPlus(AA + id * rand());
		mt_A_PSO[id].jump();
	}
}
/***********************************************************************************/
PSO::~PSO() {
	delete[] mt_A_PSO;
	delete[] sum;
	delete[] sq_sum;
	delete[] nTH_BSL;
	delete[] nTH_BTS;
	delete[] aSNR;
	delete[] uSNR;
	delete[] araSNR;
	if (tmpP != NULL) {
		tmpP = Free_Matrix_2D(tmpP);
	}
	if (P_MEAN != NULL) {
		P_MEAN = Free_Matrix_2D(P_MEAN);
	}
	if (L_BEST != NULL) {
		L_BEST = Free_Matrix_2D(L_BEST);
	}
	if (G_BEST != NULL) {
		G_BEST = Free_Matrix_2D(G_BEST);
	}
	if (G_WRST != NULL) {
		G_WRST = Free_Matrix_2D(G_WRST);
	}

	if (dPUB != NULL) {
		dPUB = Free_Matrix_2D(dPUB);
	}
	if (P_BEST != NULL) {
		P_BEST = Free_Matrix_2D(P_BEST);
	}
	if (VEL != NULL) {
		VEL = Free_Matrix_2D(VEL);
	}
}
/*******************************************************************************/
void PSO::init_HAFIZA() {
	mt_A_PSO = new XoshiroCpp::Xoshiro256PlusPlus[nProcessors];
	sum = new double[nProcessors];
	sq_sum = new double[nProcessors];
	nTH_BSL = new MY::uint[nProcessors];
	nTH_BTS = new MY::uint[nProcessors];
	aSNR = new double[nDIM];
	uSNR = new double[nDIM];
	araSNR = new double[nDIM];

	MY::uint DEL = (nPUB / nProcessors);
	nTH_BSL[0] = 0;
	nTH_BTS[0] = DEL;
	for (MY::uint id = 1; id < nProcessors; id++) {
		nTH_BSL[id] = nTH_BSL[id - 1] + DEL;
		nTH_BTS[id] = nTH_BTS[id - 1] + DEL;
		if (nTH_BTS[id] > nPUB) {
			nTH_BTS[id] = nPUB;
			std::cout << "nTH_BTS HATASI\n";
			system("pause");
			break;
		}
	}

	for (MY::uint j = 0; j < nDIM; j++) {
		aSNR[j] = 0;
		uSNR[j] = 0;
		araSNR[j] = 0;
	}

	tmpP = Init_Matrix_2D<double>(nProcessors + 1, (nTAM + 1));
	P_MEAN = Init_Matrix_2D<double>(nProcessors + 1, (nTAM + 1));
	L_BEST = Init_Matrix_2D<double>(nProcessors + 1, (nTAM + 1));
	G_BEST = Init_Matrix_2D<double>(nProcessors + 1, (nTAM + 1));
	G_WRST = Init_Matrix_2D<double>(nProcessors + 1, (nTAM + 1));
	dPUB = Init_Matrix_2D<double>(nPUB, (nTAM + 1));
	P_BEST = Init_Matrix_2D<double>(nPUB, (nTAM + 1));
	VEL = Init_Matrix_2D<double>(nPUB, nDIM);

	for (MY::uint id = 0; id < nProcessors; id++) {
		for (MY::uint j = 0; j < nDIM; j++) {
			G_BEST[id][j] = L_BEST[id][j] = 0;
		}
		for (MY::uint j = nDIM; j <= nTAM; j++) {
			G_BEST[id][j] = L_BEST[id][j] = MAX_VAL_PSO;
		}

		for (MY::uint j = 0; j < nDIM; j++) {
			G_WRST[id][j] = 0;
		}
		for (MY::uint j = nDIM; j <= nTAM; j++) {
			G_WRST[id][j] = -MAX_VAL_PSO;
		}
	}
#pragma omp parallel for
	for (int i = 0; i < (int) nPUB; i++) {
		for (MY::uint j = 0; j < nDIM; j++) {
			dPUB[i][j] = P_BEST[i][j] = VEL[i][j] = 0;
		}
		for (MY::uint j = nDIM; j <= nTAM; j++) {
			dPUB[i][j] = P_BEST[i][j] = MAX_VAL_PSO;
		}
	}
	std::cout << "init_HAFIZA() done \n";
}
/*******************************************************************************/
void PSO::SET_BEST(double *EN_IYI) {
	for (MY::uint j = 0; j < nDIM; j++) {
		G_BEST[0][j] = EN_IYI[j];
	}
	func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], false);
	std::cout << G_BEST[0][nDIM] << "  SET_ BEST -------------------------\n";
}
/***********************************************************************************/
void PSO::URET() {
	MY::uint SON = aDIM + LDIM;
//	for(MY::uint id=0;id<nProcessors;id++){
//		for (MY::uint j = 0; j <= nTAM; j++) {
//			dPUB  [nTH_BSL[id]][j]= G_BEST[0][j];
//			P_BEST[nTH_BSL[id]][j]= G_BEST[0][j];
//		}
//	}
#pragma omp parallel
	{
		MY::uint id = (MY::uint) omp_get_thread_num();
		if (id >= nProcessors) {
			std::cout << id << "\t" << nProcessors << "\tTHREAD SAYI HATA\n";
			system("pause");
		}
		MY::uint n = (nTH_BTS[id] - nTH_BSL[id]);
		sum[id] = 0;
		sq_sum[id] = 0;
		for (MY::uint j = aDIM; j < SON; j++) {
			P_MEAN[id][j] = 0;
		}
		func->UYGUNLUK(G_BEST[id], G_BEST[id][nDIM], false);
		func->UYGUNLUK(G_WRST[id], G_WRST[id][nDIM], false);
		bool OK = false;
		for (MY::uint i = nTH_BSL[id]; i < nTH_BTS[id]; i++) {
			if (i >= nPUB) {
				break;
			}
			/**********************************************************************************/
			/**********************************************************************************/
			if constexpr (_RAO != 0) {
				for (MY::uint j = 0; j < nDIM; j++) {
					tmpP[id][j] = dPUB[i][j];
				}
				if constexpr (_RAO == 1) {
					double r1 = DoubleFromBits(mt_A_PSO[id]());
					for (MY::uint j = aDIM; j < SON; j++) {
						tmpP[id][j] = dPUB[i][j]
								+ r1 * (G_BEST[id][j] - G_WRST[id][j]);
					}
				}
				if constexpr (_RAO == 2) {
					MY::uint m;
					do {
						m = nTH_BSL[id] + dist_I_PSO(mt_A_PSO[id]) % n;
					} while (i == m || i < nTH_BSL[id] || i >= nTH_BTS[id]
							|| i < 0 || i > nPUB);
					double r1 = DoubleFromBits(mt_A_PSO[id]());
					double r2 = DoubleFromBits(mt_A_PSO[id]());
					if (dPUB[i][nDIM] < dPUB[m][nDIM]) {
						for (MY::uint j = aDIM; j < SON; j++) {
							tmpP[id][j] = dPUB[i][j]
									+ r1 * (G_BEST[id][j] - G_WRST[id][j])
									+ r2
											* (fabs(dPUB[i][j])
													- fabs(dPUB[m][j]));
						}
					} else {
						for (MY::uint j = aDIM; j < SON; j++) {
							tmpP[id][j] = dPUB[i][j]
									+ r1 * (G_BEST[id][j] - G_WRST[id][j])
									+ r2
											* (fabs(dPUB[m][j])
													- fabs(dPUB[i][j]));
						}
					}
				}
				if constexpr (_RAO == 3) {
					MY::uint m;
					do {
						m = nTH_BSL[id] + dist_I_PSO(mt_A_PSO[id]) % n;
					} while (i == m || i < nTH_BSL[id] || i >= nTH_BTS[id]
							|| i < 0 || i > nPUB);
					double r1 = DoubleFromBits(mt_A_PSO[id]());
					double r2 = DoubleFromBits(mt_A_PSO[id]());
					if (dPUB[i][nDIM] < dPUB[m][nDIM]) {
						for (MY::uint j = aDIM; j < SON; j++) {
							tmpP[id][j] = dPUB[i][j]
									+ r1 * (G_BEST[id][j] - fabs(G_WRST[id][j]))
									+ r2 * (fabs(dPUB[i][j]) - dPUB[m][j]);
						}
					} else {
						for (MY::uint j = aDIM; j < SON; j++) {
							tmpP[id][j] = dPUB[i][j]
									+ r1 * (G_BEST[id][j] - fabs(G_WRST[id][j]))
									+ r2 * (fabs(dPUB[m][j]) - dPUB[i][j]);
						}
					}
				}
				for (MY::uint j = aDIM; j < SON; j++) {
					if (tmpP[id][j] < aSNR[j]) {
						tmpP[id][j] = aSNR[j];
					} else if (tmpP[id][j] > uSNR[j]) {
						tmpP[id][j] = uSNR[j];
					}
				}
				func->UYGUNLUK(tmpP[id], tmpP[id][nDIM], false);
				OK = COMPARE_UPDATE_B(dPUB[i], tmpP[id], true);
				if (OK) {
					NOTIFY(dPUB[i], id, i, true);
				}
			}
			/**********************************************************************************/
			/**********************************************************************************/
			if constexpr (_PSO != 0) {
				for (MY::uint j = aDIM; j < SON; j++) {
					L_BEST[id][j] = P_BEST[i][j];
				}
				for (MY::uint j = nDIM; j <= nTAM; j++) {
					L_BEST[id][j] = P_BEST[i][j];
				}
				for (MY::uint ii = 0; ii < nLOC; ii++) {
					MY::uint k = (i + ii);
					while (k >= nTH_BTS[id]) {
						k = nTH_BSL[id] + (k - nTH_BTS[id]);
					}
					COMPARE_UPDATE_B(L_BEST[id], P_BEST[k], true);
				}
				double *aVEL = &VEL[i][0] + aDIM;
				double *adPUB = &dPUB[i][0] + aDIM;
				double *aP_BEST = &P_BEST[i][0] + aDIM;
				double *aL_BEST = &L_BEST[id][0] + aDIM;
				double *aG_BEST = &G_BEST[id][0] + aDIM;
				double F1 = DoubleFromBits(mt_A_PSO[id]()) * c1;
				double F2 = DoubleFromBits(mt_A_PSO[id]()) * c2;
				double F3 = DoubleFromBits(mt_A_PSO[id]()) * c3;
				for (MY::uint j = aDIM; j < SON; j++) {
					(*aVEL) = K
							* ((*aVEL) + F1 * ((*aP_BEST) - (*adPUB))
									+ F2 * ((*aL_BEST) - (*adPUB))
									+ F3 * ((*aG_BEST) - (*adPUB)));
					if ((*aVEL) > (30)) {
						(*aVEL) = (30);
					} else if ((*aVEL) < (-30)) {
						(*aVEL) = (-30);
					}
					(*adPUB) += (*aVEL);
					if ((*adPUB) < aSNR[j]) {
						(*adPUB) = aSNR[j];
					} else if ((*adPUB) > uSNR[j]) {
						(*adPUB) = uSNR[j];
					}
					aVEL++;
					adPUB++;
					aP_BEST++;
					aL_BEST++;
					aG_BEST++;
//					VEL[i][j] = K * (VEL[i][j] +
//							F1 * (P_BEST[id][j] - dPUB[i][j]) +
//							F2 * (L_BEST[id][j] - dPUB[i][j]) +
//							F3 * (G_BEST[id][j] - dPUB[i][j]));
//						 if(VEL[i][j] > ( 30)){VEL[i][j] =( 30);}
//					else if(VEL[i][j] < (-30)){VEL[i][j] =(-30);}
//					dPUB[i][j] += VEL[i][j];
//						 if (dPUB[i][j] < aSNR[j]) { dPUB[i][j] = aSNR[j];}
//					else if (dPUB[i][j] > uSNR[j]) { dPUB[i][j] = uSNR[j];}
				}
				func->UYGUNLUK(dPUB[i], dPUB[i][nDIM], false);
				NOTIFY(dPUB[i], id, i, true);
			}
			/**********************************************************************************/
			/**********************************************************************************/
			if constexpr (_GA != 0) {
				for (MY::uint j = 0; j < nDIM; j++) {
					tmpP[id][j] = dPUB[i][j];
				}
				double U = DoubleFromBits(mt_A_PSO[id]());
				if (U < cro_rate) {
					if (U < mut_rate) {
						for (MY::uint j = aDIM; j < SON; j++) {
							tmpP[id][j] = aSNR[j]
									+ araSNR[j]
											* DoubleFromBits(mt_A_PSO[id]());
							if (tmpP[id][j] < aSNR[j]) {
								tmpP[id][j] = aSNR[j];
							} else if (tmpP[id][j] > uSNR[j]) {
								tmpP[id][j] = uSNR[j];
							}
						}
					} else {
						MY::uint m;
						do {
							m = nTH_BSL[id] + dist_I_PSO(mt_A_PSO[id]) % n;
						} while (i == m || i < nTH_BSL[id] || i >= nTH_BTS[id]
								|| i < 0 || i > nPUB);
						double cr_d = DoubleFromBits(mt_A_PSO[id]());
						double S = (DoubleFromBits(mt_A_PSO[id]()) - 0.5) * 4;
						for (MY::uint j = aDIM; j < SON; j++) {
							tmpP[id][j] = (cr_d) * dPUB[i][j]
									+ (1.0 - cr_d) * dPUB[m][j] + S;
							if (tmpP[id][j] < aSNR[j]) {
								tmpP[id][j] = aSNR[j];
							} else if (tmpP[id][j] > uSNR[j]) {
								tmpP[id][j] = uSNR[j];
							}
						}
					}
				}
				func->UYGUNLUK(tmpP[id], tmpP[id][nDIM], false);
				OK = COMPARE_UPDATE_B(dPUB[i], tmpP[id], true);
				if (OK) {
					for (MY::uint j = 0; j <= nTAM; j++) {
						P_BEST[i][j] = dPUB[i][j];
					}
					NOTIFY(dPUB[i], id, i, true);
				}
			}
			/**********************************************************************************/
			/**********************************************************************************/
			if constexpr (_GRD != 0) {
				const double TK = 2;
				const double der_W = 5;
				for (MY::uint j = 0; j < nDIM; j++) {
					tmpP[id][j] = dPUB[i][j];
				}
				MY::uint m = aDIM + dist_I_PSO(mt_A_PSO[id]) % (SON - aDIM);
				tmpP[id][m] += der_W;
				func->UYGUNLUK(tmpP[id], tmpP[id][nDIM], false);
				double der_O = (tmpP[id][nDIM] - dPUB[i][nDIM]);
				if (der_O != 0) {
					double del = TK * der_O / der_W;
					if (del > 10) {
						del = 10;
					} else if (del < -10) {
						del = -10;
					}
					tmpP[id][m] -= (del + der_W);
					if (tmpP[id][m] < aSNR[m]) {
						tmpP[id][m] = aSNR[m];
					} else if (tmpP[id][m] > uSNR[m]) {
						tmpP[id][m] = uSNR[m];
					}
				}
				func->UYGUNLUK(tmpP[id], tmpP[id][nDIM], false);
				OK = COMPARE_UPDATE_B(dPUB[i], tmpP[id], true);
				if (OK) {
					NOTIFY(dPUB[i], id, i, true);
				}
			}
			/**********************************************************************************/
			/**********************************************************************************/
			sum[id] += P_BEST[i][nDIM];
			sq_sum[id] += (P_BEST[i][nDIM] * P_BEST[i][nDIM]);
			for (MY::uint j = aDIM; j < SON; j++) {
				P_MEAN[id][j] += dPUB[i][j];
			}
		}
		func->UYGUNLUK(G_BEST[id], G_BEST[id][nDIM], false);
		func->UYGUNLUK(G_WRST[id], G_WRST[id][nDIM], false);
	}
	for (MY::uint id = 1; id < nProcessors; id++) {
		if (COMPARE_UPDATE_B(G_BEST[0], G_BEST[id], true)) {
			for (MY::uint i = 1; i < id; i++) {
				for (MY::uint j = 0; j <= nTAM; j++) {
					G_BEST[i][j] = G_BEST[0][j];
				}
			}
		}
		if (COMPARE_UPDATE_W(G_WRST[0], G_WRST[id], true)) {
			for (MY::uint i = 1; i < id; i++) {
				for (MY::uint j = 0; j <= nTAM; j++) {
					G_WRST[i][j] = G_WRST[0][j];
				}
			}
		}
	}
}
/***********************************************************************************/
double PSO::RUN(double *EN_IYI) {
	std::cout << "running ..." << std::endl;
	std::string NAME_1 = "PSO_OPT.txt";
	MY::uint nTEST = 1000;
	MY::uint nDEL = 100;
	MY::uint iS = 0;
	MY::uint iE = nTEST + iS;
	/***********************************/
	std::fstream DOSYA;
	std::string NAME = "PSO_OPT.txt";
	DOSYA.open(PATH + NAME, std::ios::in);
	if (!DOSYA.is_open()) {
		std::cout << PATH + NAME << " not opened\n";
	} else {
		std::vector<double> vTmp;
		double dTmp;
		std::string line;
		while (std::getline(DOSYA, line)) {
			std::istringstream iss(line);
			while (iss >> dTmp) {
				vTmp.push_back(dTmp);
			}
			if (vTmp.size() != nDIM + 3) {
				std::cout << "SATIR HATASI \n";
				break;
			}
			iS = vTmp[nDIM + 1] + nDEL;
			std::cout << vTmp[nDIM] << "\t" << vTmp[nDIM + 1] << "\t"
					<< vTmp[nDIM + 2] << "\t" << vTmp.size() << "\t" << iS
					<< "\n";
			vTmp.clear();
		}
		DOSYA.close();
	}
	iE = nTEST + iS;
	/***********************************/
	do {
		std::uint64_t AA =
				static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		for (MY::uint id = 0; id < nProcessors; id++) {
			mt_A_PSO[id] = Xoshiro256PlusPlus(mt_A_PSO[0]() + AA + id * rand());
			mt_A_PSO[id].jump();
		}

		if (iE >= func->nDATA) {
			iE = (func->nDATA - 1);
		}
		func->SET_LIM(iS, iE);
		//	func->INIT_RBF(func->nILK);
		func->GET_SNR(aSNR, uSNR);
		for (MY::uint j = 0; j < nDIM; j++) {
			aSNR[j] -= 50;
			uSNR[j] += 50;
			araSNR[j] = uSNR[j] - aSNR[j];
		}
		bool use_gaussian = false;
		bool GHANGED = false;
		MY::uint stage = 0;

		do {
			read_OPT();
			GHANGED = false;
			for (aDIM = 0; aDIM < nDIM; aDIM += LDIM) {
				init_POP(use_gaussian);
				for (MY::uint iter = 0; iter <= iterMAX; iter++) {
					URET();
					bool TMP = COMPARE_UPDATE_B(G_BEST[nProcessors], G_BEST[0],
							true);
					if (TMP || iter % 40 == 0) {
						if (TMP) {
							GHANGED = true;
						}
						std::cout << iS << "\t" << iter << "\t" << "stg: "
								<< stage << "\t" << "DIM: " << aDIM << "\t"
								<< G_BEST[0][nDIM] << "\t"
								<< G_BEST[0][nDIM + 1] << "\t";
						if (iter != 0) {
							if (CRITERIA(true)) {
								break;
							}
//							inject_POP();
						} else {
							std::cout << "\n";
						}
					}
				}
			}
			/**********************************************************************/
			use_gaussian = true;
			stage++;
		} while (GHANGED || stage < 2);

		std::cout << func->nDATA << "\t" << func->nILK << "\t" << func->nSON
				<< "\t" << G_BEST[0][nDIM] << "\n";
		func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], true);
		func->YAZ_OPT(PATH + NAME_1, G_BEST[0]);
		std::cout << "____________________________\n";
		if (iE >= (func->nDATA - 1)) {
			break;
		}
		iS += nDEL;
		iE += nDEL;
	} while (iS < (func->nDATA));

	for (MY::uint i = 0; i < nDIM; i++) {
		EN_IYI[i] = G_BEST[0][i];
	}
	return G_BEST[0][nDIM];
}
/*******************************************************************************/
void PSO::init_POP(const bool &use_gaussian) {
//	std::cout<<"init_POP started\n";
	func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], false);
	func->UYGUNLUK(G_WRST[0], G_WRST[0][nDIM], false);
	MY::uint SON = aDIM + LDIM;
	MY::uint EK = 0;
	/************************************************************/
	if (use_gaussian == false) {
		MY::uint i = 0;
		std::fstream DOSYA;
		std::string NAME[2] = { "old_PSO_OPT.txt", "PSO_OPT.txt" };
		for (MY::uint k = 0; k < 2; k++) {
			DOSYA.open(PATH + NAME[k], std::ios::in);
			if (!DOSYA.is_open()) {
				std::cout << PATH + NAME[k] << " not opened\n";
			} else {
				std::vector<double> vTmp;
				double dTmp;
				std::string line;
				MY::uint LINE_NO = 0;
				while (i < nPUB && std::getline(DOSYA, line)) {
					std::istringstream iss(line);
					while (iss >> dTmp) {
						vTmp.push_back(dTmp);
					}
					MY::uint iTMP = MY::uint(vTmp.size());
					if (k == 1) {
						iTMP -= 3;
					}
					LINE_NO++;
					if (nDIM != iTMP) {
						std::cout << "SIZE MISMATCH\n";
						std::cout << k << "\t" << NAME[k] << "\t" << LINE_NO
								<< "\n";
						std::cout << nDIM << "\t" << iTMP << "\n";
						std::cout << line << "\n";
//						#if defined(_MSC_VER)
//							system("pause");
//						#elif defined(__GNUC__)
//							int sil=0;
//							std::cout<<"press 1 to continue\n";
//							std::cin>>sil;
//						#endif
					}
					if (iTMP > nDIM) {
						iTMP = nDIM;
					}
					for (MY::uint j = 0; j < iTMP; j++) {
						dPUB[i][j] = vTmp[j];
					}
//					MY::uint s=i;
//					if(TEKRAR(dPUB,dPUB[i],0, s,nPUB,nDIM)==true){i--;}
					func->UYGUNLUK(dPUB[i], dPUB[i][nDIM], false);
					COMPARE_UPDATE_B(G_BEST[0], dPUB[i], true);
					COMPARE_UPDATE_W(G_WRST[0], dPUB[i], true);
					vTmp.clear();
					i++;
					if (i >= nPUB) {
						break;
					}
				}
				DOSYA.close();
			}
			if (i >= nPUB) {
				break;
			}
		}
		EK = i;
	}	//else{read_OPT();}
	/************************************************************/
#pragma omp parallel
	{
		MY::uint id = (MY::uint) omp_get_thread_num();
		if (id >= nProcessors) {
			std::cout << id << "\t" << nProcessors << "\tTHREAD SAYI HATA\n";
			system("pause");
		}
		for (MY::uint j = 0; j <= nTAM; j++) {
			G_BEST[id][j] = G_BEST[0][j];
			G_WRST[id][j] = G_WRST[0][j];
		}
		for (MY::uint j = aDIM; j < SON; j++) {
			dPUB[nTH_BSL[id] + 0][j] = aSNR[j];
			dPUB[nTH_BSL[id] + 1][j] = uSNR[j];
			dPUB[nTH_BSL[id] + 2][j] = 0;
		}
		for (MY::uint i = nTH_BSL[id] + 3; i < nTH_BTS[id]; i++) {
			if (i < EK) {
				continue;
			}
			if (i >= nPUB) {
				break;
			}
			for (MY::uint j = 0; j < aDIM; j++) {
				dPUB[i][j] = G_BEST[id][j];
			}
			for (MY::uint j = SON; j <= nTAM; j++) {
				dPUB[i][j] = G_BEST[id][j];
			}
//			for(MY::uint j=aDIM;j<SON;j++){
//				if(use_gaussian==true)	{dPUB[i][j] = G_BEST[id][j] + 100*(DoubleFromBits(mt_A_PSO[id]())-0.5);}
//				else					{dPUB[i][j] = aSNR[j] + araSNR[j]*DoubleFromBits(mt_A_PSO[id]());}
//					 if(dPUB[i][j] < aSNR[j]){dPUB[i][j] = aSNR[j];}
//				else if(dPUB[i][j] > uSNR[j]){dPUB[i][j] = uSNR[j];}
//			}
			double N = 0;
			for (MY::uint j = aDIM; j < SON; j++) {
				dPUB[i][j] = 2 * (DoubleFromBits(mt_A_PSO[id]()) - 0.5);
				N += dPUB[i][j] * dPUB[i][j];
			}
			N = sqrt(N);
			for (MY::uint j = aDIM; j < SON; j++) {
				dPUB[i][j] = 5 * araSNR[j] * dPUB[i][j] / N;
				if (dPUB[i][j] < aSNR[j]) {
					dPUB[i][j] = aSNR[j];
				} else if (dPUB[i][j] > uSNR[j]) {
					dPUB[i][j] = uSNR[j];
				}
			}
			func->UYGUNLUK(dPUB[i], dPUB[i][nDIM], false);
			COMPARE_UPDATE_B(G_BEST[id], dPUB[i], true);
			COMPARE_UPDATE_W(G_WRST[id], dPUB[i], true);
		}
	}
	func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], false);
	func->UYGUNLUK(G_WRST[0], G_WRST[0][nDIM], false);
	for (MY::uint id = 1; id < nProcessors; id++) {
		func->UYGUNLUK(G_BEST[id], G_BEST[id][nDIM], false);
		func->UYGUNLUK(G_WRST[id], G_WRST[id][nDIM], false);
		COMPARE_UPDATE_B(G_BEST[0], G_BEST[id], true);
		COMPARE_UPDATE_W(G_WRST[0], G_WRST[id], true);
	}
	/************************************************************/
#pragma omp parallel
	{
		MY::uint id = (MY::uint) omp_get_thread_num();
		if (id >= nProcessors) {
			std::cout << id << "\t" << nProcessors << "\tTHREAD SAYI HATA\n";
			system("pause");
		}
		for (MY::uint i = nTH_BSL[id]; i < nTH_BTS[id]; i++) {
			if (i >= nPUB) {
				break;
			}
			for (MY::uint j = 0; j < nDIM; j++) {
				VEL[i][j] = 2 * (DoubleFromBits(mt_A_PSO[id]()) - 0.5);
			}
			for (MY::uint j = 0; j < aDIM; j++) {
				dPUB[i][j] = G_BEST[0][j];
			}
			for (MY::uint j = SON; j <= nTAM; j++) {
				dPUB[i][j] = G_BEST[0][j];
			}
			func->UYGUNLUK(dPUB[i], dPUB[i][nDIM], false);
			for (MY::uint j = 0; j <= nTAM; j++) {
				P_BEST[i][j] = dPUB[i][j];
			}
		}
	}
	func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], false);
	func->UYGUNLUK(G_WRST[0], G_WRST[0][nDIM], false);
	for (MY::uint id = 1; id < nProcessors + 1; id++) {
		for (MY::uint j = 0; j <= nTAM; j++) {
			G_BEST[id][j] = G_BEST[0][j];
			G_WRST[id][j] = G_WRST[0][j];
		}
	}
//	std::cout<<"init_POP done\n";
}
/***********************************************************************************/
void PSO::inject_POP() {
	MY::uint SON = aDIM + LDIM;
	double tmp = 1.0 / (double) nPUB;
	for (MY::uint id = 1; id < nProcessors; id++) {
		for (MY::uint j = aDIM; j < SON; j++) {
			P_MEAN[0][j] += P_MEAN[id][j];
		}
	}
	for (MY::uint j = aDIM; j < SON; j++) {
		P_MEAN[0][j] *= tmp;
	}
#pragma omp parallel
	{
		MY::uint id = (MY::uint) omp_get_thread_num();
		if (id >= nProcessors) {
			std::cout << id << "\t" << nProcessors << "\tTHREAD SAYI HATA\n";
			system("pause");
		}
		MY::uint i_son = nTH_BSL[id] + (nTH_BTS[id] - nTH_BSL[id]) * 5 / 100;
		func->UYGUNLUK(G_BEST[id], G_BEST[id][nDIM], false);
		func->UYGUNLUK(G_WRST[id], G_WRST[id][nDIM], false);
		for (MY::uint i = nTH_BSL[id]; i < i_son; i += 2) {
			MY::uint k = i + 1;
			if (i < 0 || k >= (nPUB - 1)) {
				break;
			}

			for (MY::uint j = aDIM; j < SON; j++) {
				dPUB[i][j] = G_BEST[0][j]
						+ 100 * (DoubleFromBits(mt_A_PSO[id]()) - 0.5);
				if (dPUB[i][j] < aSNR[j]) {
					dPUB[i][j] = aSNR[j];
				} else if (dPUB[i][j] > uSNR[j]) {
					dPUB[i][j] = uSNR[j];
				}

				dPUB[k][j] = P_MEAN[0][j]
						+ 100 * (DoubleFromBits(mt_A_PSO[id]()) - 0.5);
				if (dPUB[k][j] < aSNR[j]) {
					dPUB[k][j] = aSNR[j];
				} else if (dPUB[k][j] > uSNR[j]) {
					dPUB[k][j] = uSNR[j];
				}
			}
			func->UYGUNLUK(dPUB[i], dPUB[i][nDIM], false);
			func->UYGUNLUK(dPUB[k], dPUB[k][nDIM], false);
			COMPARE_UPDATE_B(P_BEST[i], dPUB[i], true);
			COMPARE_UPDATE_B(P_BEST[k], dPUB[k], true);
			COMPARE_UPDATE_B(G_BEST[id], dPUB[i], true);
			COMPARE_UPDATE_B(G_BEST[id], dPUB[k], true);

			COMPARE_UPDATE_W(G_WRST[id], dPUB[i], true);
			COMPARE_UPDATE_W(G_WRST[id], dPUB[k], true);
		}
	}
	for (MY::uint id = 1; id < nProcessors; id++) {
		COMPARE_UPDATE_B(G_BEST[0], G_BEST[id], true);
		COMPARE_UPDATE_W(G_WRST[0], G_WRST[id], true);
	}
	for (MY::uint id = 1; id < nProcessors; id++) {
		for (MY::uint j = 0; j <= nTAM; j++) {
			G_BEST[id][j] = G_BEST[0][j];
			G_WRST[id][j] = G_WRST[0][j];
		}
	}
//	std::cout<<"inject_POP done\n";
}
/***********************************************************************************/
void PSO::read_OPT() {
	func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], false);
	func->UYGUNLUK(G_WRST[0], G_WRST[0][nDIM], false);
	double *TEST = new double[nTAM + 1];
	double *BEST = new double[nTAM + 1];
	double *WRST = new double[nTAM + 1];
	for (MY::uint j = 0; j <= nTAM; j++) {
		BEST[j] = G_BEST[0][j];
		WRST[j] = G_WRST[0][j];
	}
	std::fstream DOSYA;
	std::string NAME[2] = { "old_PSO_OPT.txt", "PSO_OPT.txt" };
	for (MY::uint i = 0; i < 2; i++) {
		DOSYA.open(PATH + NAME[i], std::ios::in);
		if (!DOSYA.is_open()) {
			std::cout << PATH + NAME[i] << " not opened\n";
		} else {
			std::vector<double> vTmp;
			double dTmp;
			std::string line;
			while (std::getline(DOSYA, line)) {
				std::istringstream iss(line);
				while (iss >> dTmp) {
					vTmp.push_back(dTmp);
				}
				for (MY::uint j = 0; j < nDIM; j++) {
					TEST[j] = vTmp[j];
					if (TEST[j] < aSNR[j]) {
						TEST[j] = aSNR[j];
					} else if (TEST[j] > uSNR[j]) {
						TEST[j] = uSNR[j];
					}
				}
				func->UYGUNLUK(TEST, TEST[nDIM], false);
				COMPARE_UPDATE_B(BEST, TEST, true);
				COMPARE_UPDATE_W(WRST, TEST, true);
				vTmp.clear();
			}
			DOSYA.close();
		}
	}
	for (MY::uint j = 0; j <= nTAM; j++) {
		G_BEST[0][j] = BEST[j];
		G_WRST[0][j] = WRST[j];
	}
	if (G_BEST[0][nDIM] != BEST[nDIM]) {
		std::cout << "HATA\n";
		system("pause");
	}
	for (MY::uint id = 1; id < nProcessors; id++) {
		for (MY::uint j = 0; j <= nTAM; j++) {
			G_BEST[id][j] = G_BEST[0][j];
			G_WRST[id][j] = G_WRST[0][j];
		}
	}
	delete[] TEST;
	delete[] BEST;
	delete[] WRST;
	std::cout << "read_OPT done\n";
}
/***********************************************************************************/
bool PSO::CRITERIA(const bool verbose) {
	static double old_mean = 0;
	static double old_sd = 0;

	MY::uint SUM = 0;
	for (MY::uint id = 0; id < nProcessors; id++) {
		for (MY::uint i = nTH_BSL[id]; i < nTH_BTS[id]; i++) {
			SUM++;
		}
	}
	if (SUM != nPUB) {
		std::cout << "SUM NPUB HATASI " << SUM << "\t" << nPUB << "\n";
	}

	for (MY::uint id = 1; id < nProcessors; id++) {
		sum[0] += sum[id];
		sq_sum[0] += sq_sum[id];
		sum[id] = 0;
		sq_sum[id] = 0;
	}
	double tmp = 1.0 / (double) SUM;
	double mean = sum[0] * tmp;
	double variance = (sq_sum[0] * tmp) - (mean * mean);
	variance = variance > 0 ? variance : 0;
	double sd = sqrt(variance);

	if (verbose) {
		std::cout << "M " << mean << " S " << sd << "\n";
	}

	func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], false);
	if (mean < (G_BEST[0][nDIM] - 0.0001)) {
		std::cout << "\t MEAN HATASI\n";
//		system("pause");
	}
	if (sd <= 1E-6
			|| (old_mean == mean && old_sd == sd && sd < fabs(mean) * 0.001)) {
		return true;
	}
	old_mean = mean;
	old_sd = sd;
	sum[0] = 0;
	sq_sum[0] = 0;
	return false;
}
/***********************************************************************************/
double std_dev2(double a[], int n) {
	if (n == 0) {
		return 0.0;
	}
	double sum = 0;
	double sq_sum = 0;
	for (int i = 0; i < n; ++i) {
		sum += a[i];
		sq_sum += a[i] * a[i];
	}
	double mean = sum / n;
	double variance = sq_sum / n - mean * mean;
	return sqrt(variance);
}
/***********************************************************************************/
inline bool PSO::COMPARE_UPDATE_B(double *_In1, const double *_In2,
		const bool UPDATE) {
	if (_In1[nDIM] > _In2[nDIM]
			|| (_In1[nDIM] >= _In2[nDIM] && _In1[nDIM + 1] > _In2[nDIM + 1])) {
		if (UPDATE) {
			for (MY::uint j = 0; j <= nTAM; j++) {
				_In1[j] = _In2[j];
			}
		}
		return true;
	}
	return false;
}
/***********************************************************************************/
inline bool PSO::COMPARE_UPDATE_W(double *_In1, const double *_In2,
		const bool UPDATE) {
	if (_In1[nDIM] < _In2[nDIM]
			|| (_In1[nDIM] <= _In2[nDIM] && _In1[nDIM + 1] < _In2[nDIM + 1])) {
		if (UPDATE) {
			for (MY::uint j = 0; j <= nTAM; j++) {
				_In1[j] = _In2[j];
			}
		}
		return true;
	}
	return false;
}
/***********************************************************************************/
inline bool PSO::NOTIFY(double *_In, const MY::uint &_id, const MY::uint &_i,
		const bool verbose) {
	COMPARE_UPDATE_W(G_WRST[_id], _In, true);
	COMPARE_UPDATE_B(P_BEST[_i], _In, true);
	bool OK2 = COMPARE_UPDATE_B(G_BEST[_id], _In, true);
	if (OK2 && verbose) {
#pragma omp critical
		{
			ALLERT(YESIL);
			std::cout << G_BEST[0][nDIM] << "\t" << G_BEST[_id][nDIM] << "\t"
					<< _In[nDIM] << "\n" << G_BEST[0][nDIM + 1] << "\t"
					<< G_BEST[_id][nDIM + 1] << "\t" << _In[nDIM + 1]
					<< "\t id:" << _id << "\t" << _i << "\n";
			ALLERT(KIRMIZI);
			func->UYGUNLUK(G_BEST[0], G_BEST[0][nDIM], true);
			ALLERT(YESIL);
			func->UYGUNLUK(G_BEST[_id], G_BEST[_id][nDIM], true);
			ALLERT(0);
		}
	}
	return OK2;
}
/***********************************************************************************/
inline void PSO::ALLERT(const int RENK) {
	if (RENK == YESIL) {
		std::cout << "\7" << "\033[1;92m" << std::flush;
	} // bell yeşil}
	else if (RENK == KIRMIZI) {
		std::cout << "\7" << "\033[1;91m" << std::flush;
	} // bell kırmızı
	else {
		std::cout << "\033[0m" << "\n";
	} // reset
//#if defined(__GNUC__)
//	system("play -q -n synth 0.1 sin 880 >& /dev/null");
//#endif
}
/***********************************************************************************/
/*
 reset             0  (everything back to normal)
 bold/bright       1  (often a brighter shade of the same colour)
 underline         4
 inverse           7  (swap foreground and background colours)
 bold/bright off  21
 underline off    24
 inverse off      27
 foreground background
 Black			30		40		Bright Black	90		100
 Red				31		41		Bright Red		91		101
 Green			32		42		Bright Green	92		102
 Yellow			33		43		Bright Yellow	93		103
 Blue			34		44		Bright Blue		94		104
 Magenta			35		45		Bright Magenta	95		105
 Cyan			36		46		Bright Cyan		96		106
 White			37		47		Bright White	97		107
 */
