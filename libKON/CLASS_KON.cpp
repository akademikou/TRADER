namespace MY {
typedef unsigned int uint;
}
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>
#include <cmath>
#include "MATRIX.h"
#include "DEFINITIONS.h"
#include "CLASS_FLC.h"
#include "CLASS_KON.h"

#define TxnFees		0.005
#define STOP_LOSS	(-0.01)
#define TEKE_PROF	( 0.02)

constexpr const MY::uint nOUT = 3;
constexpr const MY::uint nPSS = 5;
constexpr const MY::uint nH = nOUT * nPSS;
constexpr const MY::uint VER = 5;
#define  with_EXP
//	#define  with_SL
/***********************************************************************************/
KON::~KON() {
	if (OHLC != NULL) {
		Free_Matrix_2D(OHLC);
		OHLC = NULL;
	}
	if (DATA != NULL) {
		Free_Matrix_2D(DATA);
		DATA = NULL;
	}
	if (EMIR != NULL) {
		Free_Matrix_2D(EMIR);
		EMIR = NULL;
	}
	if (tRSI != NULL) {
		Free_Matrix_1D(tRSI);
		tRSI = NULL;
	}
}
/***********************************************************************************/
KON::KON() {
	nTH = omp_get_max_threads();
	W_RBF = NULL;
	C_RBF = NULL;

	ENTRY_PRICE = new double[nTH];
	CNT = new MY::uint[nTH];
	EXT = new MY::uint[nTH];
	YSK = new MY::uint[nTH];
//	system("cls");
	for (MY::uint i = 0; i < nTH; i++) {
		ENTRY_PRICE[i] = 0;
		CNT[i] = 0;
		EXT[i] = 0;
	}
//	std::cout.setf(std::ios::fixed, std::ios::floatfield);
//	std::cout << std::setprecision(6);
	std::vector<std::vector<double>> rawDATA;
//	T_STAMP	Op/Hi/Lo/Cl						Volume	ClCl	MAX_RET		PCZ			EMAC	[ 0 -  9]
//	SMAC	RSIC	EMA		SMA		SAR		BB_MID	RS		PC			HISTOGRAM	OBV		[10 - 19]
//	ROC		STOCH	RSI		WPR		MFI		CCI		ATR		ADX			ARN			STD		[20 - 29]
//	OSC		oEMAC	oSMAC	oRSEC	oRSIC	oSTC	oRSI	oWPR		oMFI		oPCZ	[30 - 39]
//	oADX	oARN	oOSC		        													[40 - 42]
	std::string NAME = "ASSET_TI.txt";
	std::vector<std::vector<double>> TI_DATA;
	OKU_DATA(PATH + NAME, rawDATA);
	GET_DATA(TI_DATA, rawDATA);
	nDATA = (MY::uint) TI_DATA.size();
	nINP = (MY::uint) TI_DATA[0].size();

	nILK = 0;
	nSON = nDATA;

	OHLC = Init_Matrix_2D<double>(nDATA, 6);
	DATA = Init_Matrix_2D<double>(nDATA, nINP);
	EMIR = Init_Matrix_2D<int>(nTH, nDATA);
	tRSI = Init_Matrix_1D<double>(nDATA);

	for (MY::uint i = 0; i < nDATA; i++) {
		for (MY::uint j = 0; j < 6; j++) {
			OHLC[i][j] = rawDATA[i][j + 1];
		}
		for (MY::uint j = 0; j < nINP; j++) {
			DATA[i][j] = TI_DATA[i][j];
		}
		tRSI[i] = (rawDATA[i][RSI] + 100) * 0.5;

	}
	rawDATA.clear();
	TI_DATA.clear();

//	double NORM=0;
//	for(MY::uint i=0;i<nDATA;i++){
//		while(NORM == 0){NORM = OHLC[i][3];	i++;};
//		if(NORM!=0){OHLC[i-1][3]/=NORM;}
//	}
	double NORM = OHLC[1000][3];
	for (MY::uint i = 0; i < nDATA; i++) {
		OHLC[i][3] /= NORM;
	}

	for (MY::uint i = 0; i < nTH; i++) {
		ENTRY_PRICE[i] = 0;
		CNT[i] = 0;
		EXT[i] = 0;
		YSK[i] = 0;
		for (MY::uint j = 0; j < nDATA; j++) {
			EMIR[i][j] = 0;
		}
	}
	nDIM = nH * nINP;
	/*	nKAT = 2;
	 MY::uint YAPI[]={nINP,4,3};
	 nGIR = Init_Matrix_1D<MY::uint>(nKAT);
	 nCIK = Init_Matrix_1D<MY::uint>(nKAT);

	 MY::uint maxGIR=0;
	 MY::uint maxCIK=0;
	 nDIM=0;
	 for (MY::uint i = 0; i < nKAT; i++) {
	 nGIR[i] = YAPI[i]+2;
	 nCIK[i] = YAPI[i+1];
	 nDIM+=nGIR[i]*nCIK[i];
	 if(maxGIR<nGIR[i]){maxGIR=nGIR[i];}
	 if(maxCIK<nCIK[i]){maxCIK=nGIR[i];}
	 }
	 MAP=NULL; MAP=	Init_Matrix_3D<MY::uint>(nKAT  ,maxCIK,maxGIR);
	 for (MY::uint i=0,id=0; i<nKAT; i++) {
	 for (MY::uint j=0; j<nCIK[i]; j++) {
	 for (MY::uint k=0; k<nGIR[i]; k++) {MAP[i][j][k]=id;id++;}
	 }
	 }
	 */
}
/***********************************************************************************/
void KON::GET_DATA(std::vector<std::vector<double>> &_DATA,
		const std::vector<std::vector<double>> &_rawDATA) {

	MY::uint nDATA = (MY::uint) _rawDATA.size();
	_DATA.resize(nDATA);
	std::vector<double> Tmp;
	for (MY::uint i = 3; i < nDATA; i++) {
		double tmp1 = (_rawDATA[i][RSI] - _rawDATA[i - 1][RSI]) * 2;
		double tmp2 = (_rawDATA[i - 1][RSI] - _rawDATA[i - 2][RSI]) * 2;
		double tmp3 = (_rawDATA[i - 2][RSI] - _rawDATA[i - 3][RSI]) * 2;
		_DATA[i].clear();
		/**************************************************/
		if constexpr (VER == 1) {
			_DATA[i].push_back(_rawDATA[i][RSIC]);	// 1
			_DATA[i].push_back(_rawDATA[i][RSI]);	// 2
			_DATA[i].push_back(_rawDATA[i][WPR]);	// 3
			_DATA[i].push_back(_rawDATA[i][CCI]);	// 4
			_DATA[i].push_back(_rawDATA[i][ADX]);	// 5
			_DATA[i].push_back(tmp1);					// 6

			_DATA[i].push_back(_rawDATA[i - 1][RSIC]);	// 7
			_DATA[i].push_back(_rawDATA[i - 1][RSI]);	// 8
			_DATA[i].push_back(_rawDATA[i - 1][WPR]);	// 9
			_DATA[i].push_back(_rawDATA[i - 1][CCI]);	// 10
			_DATA[i].push_back(_rawDATA[i - 1][ADX]);	// 11
			_DATA[i].push_back(tmp2);					// 12

			_DATA[i].push_back(_rawDATA[i - 2][RSIC]);	// 13
			_DATA[i].push_back(_rawDATA[i - 2][RSI]);	// 14
			_DATA[i].push_back(_rawDATA[i - 2][WPR]);	// 15
			_DATA[i].push_back(_rawDATA[i - 2][CCI]);	// 16
			_DATA[i].push_back(_rawDATA[i - 2][ADX]);	// 17
			_DATA[i].push_back(tmp3);					// 18
		}
		/**************************************************/
		if constexpr (VER == 2) {
			_DATA[i].push_back(_rawDATA[i][RSIC]);	// 1
			_DATA[i].push_back(_rawDATA[i][RSI]);	// 2
			_DATA[i].push_back(_rawDATA[i][WPR]);	// 3
			_DATA[i].push_back(_rawDATA[i][CCI]);	// 4
			_DATA[i].push_back(_rawDATA[i][ADX]);	// 5
			_DATA[i].push_back(tmp1); 					// 6

			_DATA[i].push_back(_rawDATA[i - 1][RSIC]);	// 7
			_DATA[i].push_back(_rawDATA[i - 1][RSI]);	// 8
			_DATA[i].push_back(_rawDATA[i - 1][WPR]);	// 9
			_DATA[i].push_back(_rawDATA[i - 1][CCI]);	// 10
			_DATA[i].push_back(_rawDATA[i - 1][ADX]);	// 11
			_DATA[i].push_back(tmp2);					// 12
			_DATA[i].push_back(100);					// 13
		}
		/**************************************************/
		if constexpr (VER == 3) {
			_DATA[i].push_back(_rawDATA[i][RSIC]);	// 1
			_DATA[i].push_back(_rawDATA[i][RSI]);	// 2
			_DATA[i].push_back(_rawDATA[i][WPR]);	// 3
			_DATA[i].push_back(_rawDATA[i][CCI]);	// 4
			_DATA[i].push_back(_rawDATA[i][ADX]);	// 5
			_DATA[i].push_back(tmp1); 					// 6

			_DATA[i].push_back(_rawDATA[i][RSIC]);	// 7
			_DATA[i].push_back(_rawDATA[i][RSI]);	// 8
			_DATA[i].push_back(_rawDATA[i][WPR]);	// 9
			_DATA[i].push_back(_rawDATA[i][CCI]);	// 10
			_DATA[i].push_back(_rawDATA[i][ADX]);	// 11
			_DATA[i].push_back(tmp1);					// 12

			_DATA[i].push_back(_rawDATA[i][RSIC]);	// 13
			_DATA[i].push_back(_rawDATA[i][RSI]);	// 14
			_DATA[i].push_back(_rawDATA[i][WPR]);	// 15
			_DATA[i].push_back(_rawDATA[i][CCI]);	// 16
			_DATA[i].push_back(_rawDATA[i][ADX]);	// 17
			_DATA[i].push_back(tmp1);					// 18
		}
		/**************************************************/
		if constexpr (VER == 4) {
			_DATA[i].push_back(_rawDATA[i][RSIC]);	// 1
			_DATA[i].push_back(_rawDATA[i][RSI]);	// 2
			_DATA[i].push_back(_rawDATA[i][WPR]);	// 3
			_DATA[i].push_back(_rawDATA[i][CCI]);	// 4
			_DATA[i].push_back(_rawDATA[i][ADX]);	// 5
			_DATA[i].push_back(0);						// 6

			_DATA[i].push_back(_rawDATA[i - 1][RSIC]);	// 7
			_DATA[i].push_back(_rawDATA[i - 1][RSI]);	// 8
			_DATA[i].push_back(_rawDATA[i - 1][WPR]);	// 9
			_DATA[i].push_back(_rawDATA[i - 1][CCI]);	// 10
			_DATA[i].push_back(_rawDATA[i - 1][ADX]);	// 11
			_DATA[i].push_back(0);						// 12

			_DATA[i].push_back(_rawDATA[i - 2][RSIC]);	// 13
			_DATA[i].push_back(_rawDATA[i - 2][RSI]);	// 14
			_DATA[i].push_back(_rawDATA[i - 2][WPR]);	// 15
			_DATA[i].push_back(_rawDATA[i - 2][CCI]);	// 16
			_DATA[i].push_back(_rawDATA[i - 2][ADX]);	// 17
			_DATA[i].push_back(0);						// 18
		}
		/**************************************************/
		if constexpr (VER == 5) {
			_DATA[i].push_back(_rawDATA[i][RSIC]);	// 1
			_DATA[i].push_back(_rawDATA[i][RSI]);	// 2
			_DATA[i].push_back(_rawDATA[i][WPR]);	// 3
			_DATA[i].push_back(_rawDATA[i][CCI]);	// 4
			_DATA[i].push_back(_rawDATA[i][ADX]);	// 5
			_DATA[i].push_back(tmp1);					// 6

			_DATA[i].push_back(_rawDATA[i - 1][RSIC]);	// 7
			_DATA[i].push_back(_rawDATA[i - 1][RSI]);	// 8
			_DATA[i].push_back(_rawDATA[i - 1][WPR]);	// 9
			_DATA[i].push_back(_rawDATA[i - 1][CCI]);	// 10
			_DATA[i].push_back(_rawDATA[i - 1][ADX]);	// 11
			_DATA[i].push_back(tmp2);					// 12

			_DATA[i].push_back(_rawDATA[i - 2][RSIC]);	// 13
			_DATA[i].push_back(_rawDATA[i - 2][RSI]);	// 14
			_DATA[i].push_back(_rawDATA[i - 2][WPR]);	// 15
			_DATA[i].push_back(_rawDATA[i - 2][CCI]);	// 16
			_DATA[i].push_back(_rawDATA[i - 2][ADX]);	// 17
			_DATA[i].push_back(tmp3);					// 18
			_DATA[i].push_back(0);						// 19
		}
		/**************************************************/
	}
	_DATA[0].clear();
	_DATA[1].clear();
	_DATA[2].clear();
	_DATA[3].clear();
	for (MY::uint j = 0; j < (MY::uint) _DATA[4].size(); j++) {
		_DATA[0].push_back(0);
		_DATA[1].push_back(0);
		_DATA[2].push_back(0);
		_DATA[3].push_back(0);
	}
	for (MY::uint j = 0; j < (MY::uint) _DATA[0].size(); j++) {
		double ENB = -1E5;
		double ENK = 1E5;
		for (MY::uint i = 0; i < (MY::uint) _DATA.size(); i++) {
			if (ENB < _DATA[i][j]) {
				ENB = _DATA[i][j];
			}
			if (ENK > _DATA[i][j]) {
				ENK = _DATA[i][j];
			}
		}
		ENK = fabs(ENK);
		if (ENB < ENK) {
			ENB = ENK;
		}
		for (MY::uint i = 0; i < (MY::uint) _DATA.size(); i++) {
			if (ENB != 0) {
				_DATA[i][j] = (100 * _DATA[i][j] / ENB);
			}
		}
	}
}
/***********************************************************************************/
void KON::INIT_RBF(MY::uint id) {
	// ÇALIŞTIRIRKEN KONTROL ET NINP VE DIMLAR!!!!!!!!!!!!!!!!!!!
	MY::uint i, j;
	MY::uint n = nSON - nILK;
	MY::uint c = nDIM;
	MY::uint dim = nINP;

	if (W_RBF != NULL) {
		Free_Matrix_2D(W_RBF);
		W_RBF = NULL;
	}
	if (C_RBF != NULL) {
		Free_Matrix_2D(C_RBF);
		C_RBF = NULL;
	}
	W_RBF = Init_Matrix_2D<double>(nDATA, c);
	C_RBF = Init_Matrix_2D<double>(c, dim);

	double **_X = Init_Matrix_2D<double>(n, dim);
	for (i = 0; i < n; i++) {
		for (j = 0; j < dim; j++) {
			_X[i][j] = DATA[nILK + i][j];
		}
	}
	FLC myFLC(n, c, dim, _X);
	myFLC.RUN();
	myFLC.YAZ(id);
	for (i = 0; i < n; i++) {
		for (j = 0; j < c; j++) {
			W_RBF[nILK + i][j] = myFLC.W[i][j];
		}
	}
	for (i = 0; i < c; i++) {
		for (j = 0; j < dim; j++) {
			C_RBF[i][j] = myFLC.C[i][j];
		}
	}
	if (_X != NULL) {
		Free_Matrix_2D(_X);
		_X = NULL;
	}
}
/***********************************************************************************/
void KON::KARAR_MLP(const double *_In, int &_oEMIR, const MY::uint &_i,
		double &_net) {
	double L = 0.2;	// 2*L=0.2
	double IO[8][30];
	for (MY::uint j = 0; j < nGIR[0] - 2; j++) {
		IO[0][j] = DATA[_i][j];
	}
	for (MY::uint i = 0; i < nKAT; i++) {
		IO[i][nGIR[i] - 2] = 100.0;
		IO[i][nGIR[i] - 1] = double(_oEMIR * 100.0);
		for (MY::uint j = 0; j < nCIK[i]; j++) {
			double net = 0;
			for (MY::uint k = 0; k < nGIR[i]; k++) {
				net += (_In[MAP[i][j][k]] * IO[i][k]);
			}
			IO[i + 1][j] = 100 * (2 / (1 + exp(-L * net)) - 1);
		}
	}
//	_net= 100*(IO[nKAT][1]-IO[nKAT][0]);
	if (IO[nKAT][0] > IO[nKAT][1] && IO[nKAT][0] > IO[nKAT][2]) {
		_net = 100;
	} else if (IO[nKAT][1] > IO[nKAT][0] && IO[nKAT][1] > IO[nKAT][2]) {
		_net = 0;
	} else if (IO[nKAT][2] > IO[nKAT][0] && IO[nKAT][2] > IO[nKAT][1]) {
		_net = -100;
	} else {
		_net = 0;
	}
}
/***********************************************************************************/
void KON::KARAR_SLP(const double *_In, int &_oEMIR, const MY::uint &_i,
		double &_net) {
	_net = 100.0 * (_In[nDIM - 2] + double(_oEMIR) * _In[nDIM - 1]);
	for (MY::uint j = 0; j < nINP; j++) {
		_net += DATA[_i][j] * _In[j];
	}
}
/***********************************************************************************/
void KON::KARAR_RBF(const double *_In, int &_oEMIR, const MY::uint &_i,
		double &_net) {
	_net = 100.0 * (_In[nDIM - 2] + double(_oEMIR) * _In[nDIM - 1]);
	for (MY::uint j = 0; j < nDIM - 2; j++) {
		_net += 100 * W_RBF[_i][j] * _In[j];
	}
}
/***********************************************************************************/
void KON::KARAR_LVQ(const double *_In, const int &_oEMIR, const MY::uint &_i,
		double &_net, MY::uint &_info) {
	double ENK = 1e100;
	MY::uint ind = 0;
	MY::uint ILK = 0;
	MY::uint SON = nINP;
	if constexpr (VER == 1) {
		SON = nINP;
	}
	if constexpr (VER == 2) {
		SON = nINP - 1;
	}
	if constexpr (VER == 3) {
		if (_oEMIR == -1) {
			ILK = nINP * 0 / 3;
			SON = nINP * 1 / 3;
		} else if (_oEMIR == 0) {
			ILK = nINP * 1 / 3;
			SON = nINP * 2 / 3;
		} else if (_oEMIR == 1) {
			ILK = nINP * 2 / 3;
			SON = nINP * 3 / 3;
		}
	}
	for (MY::uint i = 0; i < nH; i++) {
		double L1 = 0;
		const double *VERI_1 = &DATA[_i][0] + ILK;
		const double *VERI_2 = &_In[i * nINP] + ILK;
		for (MY::uint j = ILK; j < SON; j++) {
			double err = (*VERI_1) - (*VERI_2);
			L1 += (err * err);
			VERI_1++;
			VERI_2++;
		}
		if constexpr (VER == 2) {
			L1 += ((_oEMIR * 100 - (*VERI_2)) * (_oEMIR * 100 - (*VERI_2)));
		}
		if (L1 < ENK) {
			ind = i;
			ENK = L1;
		}
	}
	_info = ind;
	_net = MY::uint(ind / nPSS);
	if (_net == 0) {
		_net = (-1);
	} else if (_net == 2) {
		_net = (1);
	} else {
		_net = (0);
	}
}
/***********************************************************************************/
void KON::UYGUNLUK_FAST_LVQ(const double *_In, double &_result) {
	int nEMIR = 0;
	int oEMIR = 0;
	int nTXN = 0;
	double ADET = 0;
	double WALLET[2] = { 0, 0 };
	double SON_ADET = oEMIR;
	double ILK_PARA = OHLC[nILK][3];
	double SON_PARA = ILK_PARA - SON_ADET * ILK_PARA;
	double ARA_PARA = 0;

	for (MY::uint P = (nILK + 1); P < (nSON - 1); P++) {
		/*********************************************/ // KARAR_LVQ
		double ENK = 1e100;
		MY::uint ind = 0;
		MY::uint ILK = 0;
		MY::uint SON = nINP;
		if constexpr (VER == 2) {
			SON = nINP - 1;
		}
		if constexpr (VER == 3) {
			if (oEMIR == -1) {
				ILK = nINP * 0 / 3;
				SON = nINP * 1 / 3;
			} else if (oEMIR == 0) {
				ILK = nINP * 1 / 3;
				SON = nINP * 2 / 3;
			} else if (oEMIR == 1) {
				ILK = nINP * 2 / 3;
				SON = nINP * 3 / 3;
			}
		}
		for (MY::uint i = 0; i < nH; i++) {
			double L1 = 0;
			const double *VERI_1 = &DATA[P][0] + ILK;
			const double *VERI_2 = &_In[i * nINP] + ILK;
			for (MY::uint j = ILK; j < SON; j++) {
				double err = (*VERI_1) - (*VERI_2);
				L1 += (err * err);
				VERI_1++;
				VERI_2++;
			}
			if constexpr (VER == 2) {
				L1 += ((oEMIR * 100 - (*VERI_2)) * (oEMIR * 100 - (*VERI_2)));
			}
			if (L1 < ENK) {
				ind = i;
				ENK = L1;
			}
		}
		/*********************************************/
		MY::uint net = MY::uint(ind / nPSS);
		if (net == 0) {
			nEMIR = (-1);
		} else if (net == 2) {
			nEMIR = (1);
		} else {
			nEMIR = (oEMIR);
		}
		/*********************************************/
#ifdef with_EXP
		if (P >= 1 && nEMIR != oEMIR) {
			double new_RSI = tRSI[P];
			double old_RSI = tRSI[P - 1];
			if (nEMIR > oEMIR) {			// Bu koşullarda ALMA !!!
				if (new_RSI > 60 || new_RSI < 20 || new_RSI < old_RSI) {
					nEMIR = oEMIR;
				}
			} else {							// Bu koşullarda SATMA !!!
				if (new_RSI < 40 || new_RSI > 80 || new_RSI > old_RSI) {
					nEMIR = oEMIR;
				}
			}
		} else {
			nEMIR = oEMIR;
		}
#endif
		/*********************************************/
		if (oEMIR != nEMIR) {
			double D = OHLC[P][3]; /// yapılandırmada bir gecikme kontrol zaten
			if (D == 0.0) {
				std::cout << "HATA\n";
				system("pause");
			}
//			if(ILK_PARA == 0){ILK_PARA=SON_PARA=D;}
			if (nEMIR == 0) {
				ADET = (-SON_ADET);
			} // paranin tamami kadar al/sat
			else {
				if (nEMIR == 1) {
					ADET = (nEMIR * SON_PARA / D) / (1 + TxnFees);
				} else {
					ADET = (-2 * SON_ADET + (nEMIR * SON_PARA / D))
							/ (1 + TxnFees);
				}
			}
			double commision = fabs(ADET * D * TxnFees);
			SON_PARA = SON_PARA - ADET * D - commision;
			SON_ADET = SON_ADET + ADET;
			ADET = 0;

			WALLET[0] = (WALLET[1] == 0) ? ILK_PARA : WALLET[1];
			WALLET[1] = SON_PARA + SON_ADET * D;
			nTXN++;
		}
		if (P == (nSON - 100)) {
			ARA_PARA = SON_PARA + SON_ADET * OHLC[P][3];
		}
		oEMIR = nEMIR;
		/*********************************************/
	}
	if (SON_ADET != 0) {
		ADET = (-SON_ADET);
		double D = OHLC[nSON - 1][3];
		double commision = fabs(ADET * D * TxnFees);
		SON_PARA = SON_PARA - ADET * D - commision;
		SON_ADET = SON_ADET + ADET;
		ADET = 0;
		WALLET[0] = (WALLET[1] == 0) ? ILK_PARA : WALLET[1];
		WALLET[1] = SON_PARA + SON_ADET * D;
		nTXN++;
	}
//	double RET = (ILK_PARA==0) ? 0 : (SON_PARA/ILK_PARA)-1;
//	if(nTXN < 5){_result[0]=1E5;}else
	{
//		_result[0]= -RET;
		double K1 = (SON_PARA - ILK_PARA);
		double K2 = (SON_PARA - ARA_PARA);
		if (K1 > 0.00 && K2 > 0.00 && K2 > (K1 * 0.1)) {
			(&_result)[0] = -K1;
		} else {
			(&_result)[0] = (-K1 + 1);
		}
		(&_result)[1] = (-K2);
	}
}
/***********************************************************************************/
//int  KON::KARAR(const double* _W, const int &_oEMIR,const MY::uint &_i,const MY::uint &_id,MY::uint &_info)
int KON::KARAR(const double *_W, const int &_oEMIR, const MY::uint &_i,
		MY::uint &_info) {
	if (_i == nSON - 1) {
		return 0;
	}
	double net = 0;
	int ORD1 = 0;
//	KARAR_MLP(_W, _oEMIR,_i,net);
//	KARAR_SLP(_W, _oEMIR,_i,net);
//	KARAR_RBF(_W, _oEMIR,_i,net);
	KARAR_LVQ(_W, _oEMIR, _i, net, _info);
	if (net == 0) {
		ORD1 = _oEMIR;
	} else {
		ORD1 = EMIR_YAZ(net);
	}
	int nEMIR = EXP_KNOW(ORD1, _oEMIR, _i);
#ifdef with_SL
	ORD1=nEMIR;
	if( EXT[_id] == 1 &&
		ORD1     != 0 &&
		nEMIR    != 0 &&
		nEMIR    != YSK[_id]){
		EXT[_id]=0;
		CNT[_id]=0;
		YSK[_id]=0;
	}else{
		if( EXT[_id] == 1){
			nEMIR = 0;
			CNT[_id]++;
			if(CNT[_id]>2){ // YASAK BITIYOR
				EXT[_id]=0;
				CNT[_id]=0;
				YSK[_id]=0;
			}
		}else{
			int ORD2=ORD1;
			if(ENTRY_PRICE[_id]>0 && nEMIR != 0 && _oEMIR != 0){
				double act=(OHLC[_i][3]-ENTRY_PRICE[_id])/ENTRY_PRICE[_id];
				bool TEST = OHLC[_i][3]>OHLC[_i-1][3];
				if( (_oEMIR== ( 1) && ORD1 ==  1 && ( act < STOP_LOSS || ( act > TEKE_PROF && TEST==false))) ||
					(_oEMIR== (-1) && ORD1 == -1 && (-act < STOP_LOSS || (-act > TEKE_PROF && TEST==true))))
				{ORD2 = 0;}
//				if( (_oEMIR== ( 1) && ORD1 ==  1 && ( act < STOP_LOSS || ( act > TEKE_PROF))) ||
//					(_oEMIR== (-1) && ORD1 == -1 && (-act < STOP_LOSS || (-act > TEKE_PROF))))
//				{ORD2 = 0;}
			}
			nEMIR = EXP_KNOW(ORD2,_oEMIR,_i);
			if(nEMIR == 0 && ORD1 != 0){EXT[_id]= 1; CNT[_id]=0; YSK[_id]=ORD1;}
		}
	}
	if(nEMIR != _oEMIR && nEMIR != 0){
		EXT[_id]=0;
		CNT[_id]=0;
		YSK[_id]=0;
		ENTRY_PRICE[_id] = OHLC[_i][3];
	}
	if(nEMIR ==  0){ENTRY_PRICE[_id] = 0;}

	if( _oEMIR ==  1 && nEMIR ==  1 && ENTRY_PRICE[_id] < OHLC[_i][3]) {ENTRY_PRICE[_id] = OHLC[_i][3];}
	if( _oEMIR == -1 && nEMIR == -1 && ENTRY_PRICE[_id] > OHLC[_i][3]) {ENTRY_PRICE[_id] = OHLC[_i][3];}
#endif
	return nEMIR;
}
/***********************************************************************************/
int KON::EMIR_YAZ(const double &_net) {
	if (_net > (0.001)) {
		return (1);
	} else if (_net < (-0.001)) {
		return (-1);
	} else {
		return 0;
	}
}
/***********************************************************************************/
int KON::EXP_KNOW(const int &_nEMIR, const int &_oEMIR, const MY::uint &_i) {
#ifdef with_EXP
	if (_i >= 1 && _nEMIR != _oEMIR) {
		double new_RSI = tRSI[_i];
		double old_RSI = tRSI[_i - 1];
		if (_nEMIR > _oEMIR) {					// Bu koşullarda ALMA !!!
			if (new_RSI > 60 || new_RSI < 20 || new_RSI < old_RSI) {
				return _oEMIR;
			}
		}
		if (_nEMIR < _oEMIR) {					// Bu koşullarda SATMA !!!
			if (new_RSI < 40 || new_RSI > 80 || new_RSI > old_RSI) {
				return _oEMIR;
			}
		}
	}
#endif
	return _nEMIR;
}
/***********************************************************************************/
void KON::UYGUNLUK(const double *_In, double &_result, bool _VERBOSE) {
	if (!_VERBOSE) {
		UYGUNLUK_FAST_LVQ(_In, _result);
		return;
	}
	std::cout << "UYGUNLUK\n";
	MY::uint id = (MY::uint) omp_get_thread_num();

	ENTRY_PRICE[id] = 0;
	CNT[id] = 0;
	EXT[id] = 0;
	YSK[id] = 0;
	_result = 0;
	double S = 0;
	for (MY::uint i = 0; i < nDATA; i++) {
		EMIR[id][i] = 0;
	}
//	int YON=0;
//	for(int K=-1;K<=1;K++)
	int K = 0;
	{
		EMIR[id][nILK] = K;
		MY::uint info;
		for (MY::uint i = (nILK + 1); i < (nSON - 1); i++) {
			EMIR[id][i] = KARAR(_In, EMIR[id][i - 1], i, info);
			//		EMIR[id][i]=KARAR(_In,EMIR[id][i-1],i,id,info);
			//		EMIR[id][i-1]=YON; //LEAD YAPMAK ICIN
			//		EMIR[id][i]=YON;
			//		if(_VERBOSE){std::cout<<i<<"\t"<<EMIR[id][i]<<"\t"<<info<<"\n";}
			//		S+=EMIR[id][i];
		}
		EMIR[id][nSON - 1] = 0;
		UYGUNLUK_1(EMIR[id], _result, _VERBOSE);
		S += _result;
	}
//	UYGUNLUK_1(_In,_result,_VERBOSE);
//	UYGUNLUK_2(_In,_result,_VERBOSE);
//	UYGUNLUK_3(_In,_result,_VERBOSE);
	_result = S;
}
/***********************************************************************************/
void KON::UYGUNLUK_1(const int *_In, double &_result, bool VERBOSE) {
	MY::uint id = (MY::uint) omp_get_thread_num();
	ENTRY_PRICE[id] = 0;
	CNT[id] = 0;
	EXT[id] = 0;
	YSK[id] = 0;
	_result = 0;
	int LAST = 0;
	double nTXN = 0;
	double nWIN = 0;
	double nLOS = 0;
	double nNTR = 0;
	double gross_profits = 0;
	double gross_loss = 0;
	double Big_WIN = 0;
	double Big_LOS = 0;
	double SD = 0;
	double SD_1 = 0;
	double SD_2 = 0;

	int YON = 0;
	double ARA_PARA = 0;
	double ILK_PARA = 0;
	double SON_PARA = 0;
	double SON_ADET = 0;
	double ADET = 0;
	double D = 0;
	double WALLET[2] = { 0, 0 };
//	std::fstream DOSYA;
//	std::string NAME="INFO.txt";
//	std::vector<std::vector<double>>TI_DATA;
//	if(VERBOSE){DOSYA.open(PATH+NAME, std::ios::out);}

	LAST = _In[nILK];
	if (LAST != 0) {
		std::cout << "LAST :" << LAST << "\n";
#if defined(_MSC_VER)
			system("pause");
		#elif defined(__GNUC__)
		int sil = 0;
		std::cout << "press 1 to continue\n";
		std::cin >> sil;
#endif
	}
	SON_ADET = LAST;
	ILK_PARA = OHLC[nILK][3];
	SON_PARA = ILK_PARA - SON_ADET * ILK_PARA;

	for (MY::uint i = (nILK + 1); i < nSON; i++) {
		YON = _In[i];
//		YON=KARAR(_In,LAST,i,nSON,id);
//		if(i==(nILK)){YON=0;}
//		if(i==(nSON-1)){YON=0;}
		if (LAST != YON) {
			D = OHLC[i][3];
			if (D == 0.0) {
				std::cout << "HATA\n";
				system("pause");
			}
			if (ILK_PARA == 0) {
				ILK_PARA = SON_PARA = D;
			}
			if (YON == 0) {
				ADET = (-SON_ADET);
			}	// paranin tamami kadar al/sat
			else {
				if (YON == 1) {
					ADET = (YON * SON_PARA / D) / (1 + TxnFees);
				} else {
					ADET = (-2 * SON_ADET + (YON * SON_PARA / D))
							/ (1 + TxnFees);
				}
			}
			double commision = fabs(ADET * D * TxnFees);
			SON_PARA = SON_PARA - ADET * D - commision;
			SON_ADET = SON_ADET + ADET;
			ADET = 0;

			WALLET[0] = (WALLET[1] == 0) ? ILK_PARA : WALLET[1];
			WALLET[1] = SON_PARA + SON_ADET * D;
			double FARK = (WALLET[1] - WALLET[0]);

			nTXN++;
			if (FARK > 0) {
				gross_profits += FARK;
				nWIN++;
			} else if (FARK < 0) {
				gross_loss += fabs(FARK);
				nLOS++;
			} else {
				nNTR++;
			}
			if (Big_WIN < FARK) {
				Big_WIN = FARK;
			}
			if (Big_LOS > FARK) {
				Big_LOS = FARK;
			}
			SD_1 += (FARK * FARK);
			SD_2 += (FARK);
//			if(VERBOSE){std::cout<<i<<"\t"<<FARK<<"\t"<<WALLET[1]<<"\t"<<WALLET[0]<<"\n";}
		}
		/*	if(VERBOSE){
		 DOSYA<<i<<"\t"<<OHLC[i][3]<<"\t"<<YON<<"\t"
		 <<WALLET[0]<<"\t"<<WALLET[1]<<"\t"<<WALLET[1]-WALLET[0]<<"\t"
		 <<gross_profits<<"\t"<<gross_loss<<"\t"<<gross_profits-gross_loss<<"\t"
		 <<nWIN<<"\t"<<nLOS<<"\t"<<nTXN<<"\n";}
		 */
		LAST = YON;
		if (i == (nSON - 100)) {
			ARA_PARA = SON_PARA + SON_ADET * OHLC[i][3];
		}
	}
	if (SON_ADET != 0) {
		std::cout << "-------------" << SON_ADET << std::endl;
		if (SON_ADET > 0) {
			SON_PARA = SON_PARA + SON_ADET * OHLC[nSON - 1][3] * (1 - TxnFees);
		}
		if (SON_ADET < 0) {
			SON_PARA = SON_PARA - SON_ADET * OHLC[nSON - 1][3] * (1 - TxnFees);
		}
		nTXN++;
	}

	double Net_Profit = gross_profits - gross_loss;
	double Profit_factor = (gross_loss == 0) ? 0 : gross_profits / gross_loss;
	double Win_Ratio = (nTXN == 0) ? 0 : nWIN / nTXN;
	double Average_Winner = (nWIN == 0) ? 0 : gross_profits / nWIN;
	double Average_Looser = (nLOS == 0) ? 0 : gross_loss / nLOS;
	double Average_Profit = (nTXN == 0) ? 0 : Net_Profit / nTXN;
	double PL_Ratio =
			(Average_Looser == 0) ? 0 : Average_Winner / Average_Looser;
	double RET = (ILK_PARA == 0) ? 0 : (SON_PARA / ILK_PARA) - 1;
//	for(MY::uint i=nILK;i<nSON;i++){
//		if(GAIN_LOSS[i]!=0){SD+=(GAIN_LOSS[i]-Average_Profit)*(GAIN_LOSS[i]-Average_Profit);}
//	}
	// DIZI KULLANMAMAK ICIN a2-2ab+b2 ile hesap
	SD = (nTXN == 0) ?
			0 :
			sqrt(
					(SD_1 - 2 * Average_Profit * SD_2
							+ Average_Profit * Average_Profit * nTXN) / nTXN);
	double Sharp_Ratio = (SD == 0) ? 0 : Average_Profit / SD;

//	if(nTXN<5 || nWIN < nLOS || Average_Profit < 0){_result=1E20;}else{
	if (nTXN < 5) {
		_result = 1E5;
	} else {
		_result = Net_Profit;
//		_result= Net_Profit-Big_WIN+Big_LOS;
//		_result= Win_Ratio;
//		_result= nWIN;
//		_result= Sharp_Ratio;
//		_result= RET;
//		_result= PL_Ratio;
//		_result= Profit_factor;
		_result = (-_result);
	}
	if (VERBOSE) {
		std::cout << "[ " << nILK << "\t" << nSON << " ]\n";
		std::cout << "Net_Profit" << "\t" << Net_Profit << "\n";
		std::cout << "Net_Profit S-I" << "\t" << SON_PARA - ILK_PARA << "\n";
		std::cout << "Profit_factor" << "\t" << Profit_factor << "\n";
		std::cout << "Win_Ratio" << "\t" << Win_Ratio << "\n";
		std::cout << "Average_Winner" << "\t" << Average_Winner << "\n";
		std::cout << "Average_Looser" << "\t" << Average_Looser << "\n";
		std::cout << "Average_Profit" << "\t" << Average_Profit << "\n";
		std::cout << "PL_Ratio" << "\t" << PL_Ratio << "\n";
		std::cout << "Sharp_Ratio" << "\t" << Sharp_Ratio << "\n";
		std::cout << "Cumm_Ret" << "\t" << RET << "\n";
		std::cout << "Big_WIN\t" << "\t" << Big_WIN << "\n";
		std::cout << "Big_LOS\t" << "\t" << Big_LOS << "\n";
		std::cout << "gross_profits" << "\t" << gross_profits << "\n";
		std::cout << "gross_loss" << "\t" << gross_loss << "\n";
		std::cout << "nWIN\t" << "\t" << nWIN << "\n";
		std::cout << "nLOS\t" << "\t" << nLOS << "\n";
		std::cout << "nNTR\t" << "\t" << nNTR << "\n";
		std::cout << "nTXN\t" << "\t" << nTXN << "\n";
		std::cout << "ARA_1\t" << "\t" << ARA_PARA - ILK_PARA << "\n";
		std::cout << "ARA_2\t" << "\t" << SON_PARA - ARA_PARA << "\n";
		std::cout << "__________________\n";
//		DOSYA.close();
	}
}
/***********************************************************************************/
/*
 void KON::UYGUNLUK_2(const int* _In, double &_result) {
 _result=0;
 // YALNIZ LONG POZISYONLAR
 double RET=1;
 int oEMIR=0;
 int nEMIR=0;
 for(MY::uint i=nILK;i<nSON;i++){
 nEMIR=_In[i];
 if(nEMIR==1)			{RET*=(1+OHLC[i+1][5]);}
 if(nEMIR==1 && oEMIR==0){RET/=(1+TxnFees);}
 if(nEMIR==0 && oEMIR==1){RET*=(1-TxnFees);}
 oEMIR=nEMIR;
 }
 _result=( -(RET-1));
 }
 */
/***********************************************************************************/
/*
 void KON::UYGUNLUK_3(const int* _In, double &_result) {
 _result=0;
 double RET=1;
 int nEMIR=0;
 double ILK_PARA=0;
 double SON_PARA=0;
 double ilk=0,son=0;

 for(MY::uint i=nILK;i<nSON;i++){
 RET=1;
 MY::uint TRADE_L=0;
 MY::uint TRADE_S=0;
 nEMIR=_In[i];

 while(nEMIR == 0 && i<nSON){
 i++;
 nEMIR=_In[i];
 };
 if(ILK_PARA == 0){ILK_PARA=SON_PARA=OHLC[i+1][3];}

 while(nEMIR == 1 && i<nSON-1){
 TRADE_L=1;
 RET*=(1+OHLC[i+1][5]);
 i++;
 nEMIR=_In[i];
 };
 if(TRADE_L!=0){
 RET*=(1-TxnFees)/(1+TxnFees);
 SON_PARA *= RET;
 }

 if(nEMIR == -1){
 TRADE_S=1;
 ilk=OHLC[i+1][3];
 while(nEMIR == -1){
 if(i >= nSON){break;}else{i++;}
 nEMIR=_In[i];
 };
 son=OHLC[i+1][3];
 RET =1+(1-TxnFees)/(1+TxnFees)-(son/ilk);
 RET*=(1+TxnFees)/(1-TxnFees);
 }
 if(TRADE_S!=0){
 RET*=(1-TxnFees)/(1+TxnFees);
 SON_PARA *= RET;
 }

 }
 RET=0;
 if(ILK_PARA!=0){RET=(SON_PARA/ILK_PARA)-1;}
 _result=( -RET);
 }
 */
/***********************************************************************************/
void KON::OKU_DATA(std::string name,
		std::vector<std::vector<double>> &rawDATA) {
	double dTmp;
	std::string line;
	std::vector<double> vTmp;
	rawDATA.clear();
	std::fstream DOSYA;
	DOSYA.open(name.c_str(), std::ios::in);
	if (!DOSYA.is_open()) {
		std::cout << name.c_str() << " not opened\n";
	}
	std::getline(DOSYA, line);
	while (std::getline(DOSYA, line)) {
		std::istringstream iss(line);
		while (iss >> dTmp) {
			vTmp.push_back(dTmp);
		}
		rawDATA.push_back(vTmp);
		vTmp.clear();
	}
	DOSYA.close();
}
/***********************************************************************************/
void KON::YAZ_EMIR(const std::string NAME, const double *_In) {
	MY::uint id = (MY::uint) omp_get_thread_num();
	ENTRY_PRICE[id] = 0;
	CNT[id] = 0;
	EXT[id] = 0;
	YSK[id] = 0;
//	double _result=0;
	for (MY::uint i = 0; i < nDATA; i++) {
		EMIR[id][i] = 0;
	}
	MY::uint info;
	MY::uint SAY[nTH][nH];
	for (MY::uint i = 0; i < nTH; i++) {
		for (MY::uint j = 0; j < nH; j++) {
			SAY[i][j] = 0;
		}
	}
	for (MY::uint i = (nILK + 1); i < nSON; i++) {
		EMIR[id][i] = KARAR(_In, EMIR[id][i - 1], i, info);
//		EMIR[id][i]=KARAR(_In,EMIR[id][i-1],i,id,info);
		SAY[id][info]++;
	}
	EMIR[id][nSON - 1] = 0;
	double _result = 0;
	UYGUNLUK_1(EMIR[id], _result, true);
//	for (MY::uint j = 0; j < nH; j++) { std::cout << SAY[id][j]<<"\n"; }
	std::fstream DOSYA;
	DOSYA.open(NAME, std::ios::out);
	for (MY::uint i = 0; i < nDATA; i++) {
		DOSYA << EMIR[id][i] << "\t" << OHLC[i][3] << "\n";
	}
	DOSYA.close();
}
/***********************************************************************************/
void KON::YAZ_TAM_EMIR(const std::string NAME) {
	MY::uint id = (MY::uint) omp_get_thread_num();
	ENTRY_PRICE[id] = 0;
	CNT[id] = 0;
	EXT[id] = 0;
	YSK[id] = 0;
//	double _result=0;
	for (MY::uint i = 0; i < nDATA; i++) {
		EMIR[id][i] = 0;
	}
	MY::uint info;
	MY::uint SAY[nTH][nH];
	for (MY::uint i = 0; i < nTH; i++) {
		for (MY::uint j = 0; j < nH; j++) {
			SAY[i][j] = 0;
		}
	}
	///////////////////////////////////////////////////////////////
	double dTmp;
	std::fstream DOSYA;
	std::string line;
	std::vector<double> vTmp;
	std::vector<std::vector<double>> PSO_DATA;
	PSO_DATA.clear();
	std::string DOSYA_ADI = "PSO_OPT.txt";
	DOSYA.open(PATH + DOSYA_ADI, std::ios::in);
	if (!DOSYA.is_open()) {
		std::cout << PATH + DOSYA_ADI << " not opened\n";
	}
	while (std::getline(DOSYA, line)) {
		std::istringstream iss(line);
		while (iss >> dTmp) {
			vTmp.push_back(dTmp);
		}
		PSO_DATA.push_back(vTmp);
		vTmp.clear();
	}
	DOSYA.close();
	MY::uint nTEST = (MY::uint) PSO_DATA.size();
	MY::uint nPAR = (MY::uint) PSO_DATA[0].size();
	double **PSO_RES = Init_Matrix_2D<double>(nTEST, nPAR);
	for (MY::uint i = 0; i < (MY::uint) PSO_DATA.size(); i++) {
		for (MY::uint j = 0; j < (MY::uint) PSO_DATA[i].size(); j++) {
			PSO_RES[i][j] = PSO_DATA[i][j];
		}
		PSO_DATA[i].clear();
	}
	PSO_DATA.clear();
	double *_In;
	for (MY::uint i = 0; i < nTEST; i++) {
		double RES = PSO_RES[i][nPAR - 3];
		MY::uint ilk = (MY::uint) PSO_RES[i][nPAR - 2];
		MY::uint son = (MY::uint) PSO_RES[i][nPAR - 1];
		std::cout << i << "\t" << RES << "\t" << ilk << "\t" << son << "\n";

		_In = PSO_RES[i];
		for (MY::uint j = (son); j < (son + 100); j++) {
			if (j >= nDATA) {
				break;
			}
			EMIR[id][j] = KARAR(_In, EMIR[id][j - 1], j, info);
//			EMIR[id][j]=KARAR(_In,EMIR[id][j-1],j,id,info);
			SAY[id][info]++;
		}
	}
	nSON = nDATA;
	nILK = 0;
	nILK = (MY::uint) PSO_RES[0][nPAR - 1];
	EMIR[id][nSON - 1] = 0;
	if (PSO_RES != NULL) {
		Free_Matrix_2D(PSO_RES);
		PSO_RES = NULL;
	}
	double _result = 0;
	UYGUNLUK_1(EMIR[id], _result, true);
	DOSYA.open(NAME, std::ios::out);
	for (MY::uint i = 0; i < nDATA; i++) {
		DOSYA << EMIR[id][i] << "\t" << OHLC[i][3] << "\n";
	}
	DOSYA.close();
}
/***********************************************************************************/
void KON::YAZ_OPT(std::string NAME, double *_In) {
	std::fstream DOSYA;
	DOSYA.open(NAME, std::ios::app);
	DOSYA << std::setprecision(6);
	for (MY::uint i = 0; i < nDIM; i++) {
		DOSYA << _In[i] << "\t";
	}
	DOSYA << _In[nDIM];
	DOSYA << "\t" << nILK;
	DOSYA << "\t" << nSON;
	DOSYA << std::endl;
	DOSYA.close();
}
/***********************************************************************************/
MY::uint KON::GET_DIM() {
	return (nDIM);
}
/***********************************************************************************/
void KON::SET_LIM(const MY::uint _ILK, const MY::uint _SON) {
	nILK = _ILK;
	nSON = _SON;
}
/***********************************************************************************/
void KON::GET_SNR(double *aSNR, double *uSNR) {
	for (MY::uint j = 0; j < nINP; j++) {
		double ENB = -1E5;
		double ENK = 1E5;
		for (MY::uint i = (nILK + 1); i < nSON; i++) {
			if (ENB < DATA[i][j]) {
				ENB = DATA[i][j];
			}
			if (ENK > DATA[i][j]) {
				ENK = DATA[i][j];
			}
		}
		for (MY::uint i = 0; i < nH; i++) {
			MY::uint ind = (i * nINP) + j;
			aSNR[ind] = ENK;
			uSNR[ind] = ENB;
		}
	}
}
/***********************************************************************************/
