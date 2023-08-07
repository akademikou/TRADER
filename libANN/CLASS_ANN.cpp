#pragma warning (disable:4244)
#pragma warning (disable:4267)
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include "MATRIX.h"
#include "CLASS_ANN.h"
std::mt19937_64		mt_A;
std::random_device	rd_A;
std::uniform_real_distribution<double> dist_R;
std::uniform_int_distribution<unsigned int>	 dist_I;
/*******************************************************************************/
MDL::~MDL() {
	if (CIK != NULL) { CIK = Free_Matrix_1D(CIK); }
	if (W != NULL) { W = Free_Matrix_2D(W); }
}
/*******************************************************************************/
/*	_nGIR adet giriş _nCIK adet çıkış için
	CIK isimli çıkış değişkeni
	W isimli yol ağırlıkları
	için hafıza ayırma
	W'yi [-1 1] aralığında rastlantısal değerlerle
	ilk yapılandırma
*/
MDL::MDL(int _nGIR,int _nCIK){
	OK=0.1;
	nGIR=_nGIR;
	nCIK=_nCIK;
	CIK	=NULL;
	W	=NULL;
	CIK = Init_Matrix_1D<double>(nCIK);
	W = Init_Matrix_2D<double>(nCIK, nGIR);
	for (int i = 0; i <nCIK; i++) { 
		for (int j = 0; j < nGIR; j++) { 
			/* [-1 1] aralığında rastlantısal sayı */
			W[i][j] = 2.0*double(rand())/double(RAND_MAX)-1;
		}
	}
}
/*******************************************************************************/
/*	nGIR adet giriş nCIK adet çıkış 
	CIK= 1 eğer net>0
	CIK=-1 eğer net<0
	için
	CIK isimli çıkış dizisini
	GIR isimli giriş dizisi
	kullanarak hesaplama
	----------------------------------
	Eğer eğitim yapılacaksa;
		GERCEK=i ise yalnız i.
		çıkışın 1 diğerlerinin -1
		olması gerektiği kurgusu ile
	W isimli yol ağırlıklarını
	OK isimli öğrenme katsayısını kullanarak güncelleme
*/
int	MDL::run(double *GIR, bool TEACH, int GERCEK){
	int S=0;
	for (int  i = 0; i < nCIK; i++) {
		double net = 0;
		for (int  j = 0; j < nGIR; j++) {net+=W[i][j]*GIR[j];}
		if(net>0){CIK[i]=1;S=i;}else{CIK[i]=-1;}
		if (TEACH==true){
			double E;
			if(GERCEK==i){E=1-CIK[i];}else{E=-1-CIK[i];}
			for (int  j = 0; j < nGIR; j++) {W[i][j]+=OK*E*GIR[j];}
		}
	}
	return S;
}
/*******************************************************************************/
MLP::~MLP() {}
/*******************************************************************************/
MLP::MLP(int _nGIR,int _nCIK) {
	nKAT = 3;
	nGIR = Init_Matrix_1D<int>(nKAT);
	nCIK = Init_Matrix_1D<int>(nKAT);
	YAPI = Init_Matrix_1D<int>(nKAT+1);
	YAPI[0]=_nGIR;	
	YAPI[nKAT]=1;
	for (int i = 1; i < nKAT; i++) {
		YAPI[i] = _nCIK;
	}
//	YAPI[0] = _nGIR;
//	YAPI[1] = 5;
//	YAPI[2] = 3;
//	YAPI[3] = 1;
	int maxGIR=0;
	int maxCIK=0;
	for (int i = 0; i < nKAT; i++) {
		nGIR[i] = YAPI[i];
		nCIK[i] = YAPI[i+1];
		if(maxGIR<nGIR[i]){maxGIR=nGIR[i];}
		if(maxCIK<nCIK[i]){maxCIK=nGIR[i];}
	}
	ERR=NULL; ERR=	Init_Matrix_2D<double>(nKAT  ,maxCIK);
	IO =NULL; IO=	Init_Matrix_2D<double>(nKAT+1,maxGIR);
	W  =NULL; W=	Init_Matrix_3D<double>(nKAT  ,maxCIK,maxGIR);

	/*
	 nKAT adet katmana sahip
		i. katmanında;
			nGIR[i] adet girişe sahip
			nCIK[i] adet çıkışa sahip
	çok katmanlı yapay sinir ağında
	W isimli yol ağırlıklarını
	[-1 1] aralığında rastlantısal değerlerle
	ilk yapılandırma
	*/
	for (int i = 0; i < nKAT; i++) { 
		for (int j = 0; j < nCIK[i]; j++) { 
			for (int k = 0; k < nGIR[i]; k++) {
				W[i][j][k] = (2*double(rand())/double(RAND_MAX)-1);
			}
		}
	}
	S	= 1.0; // gaussian
	L	= 0.99; // linear, ramp, sigmoid, hyperpolic act
	L1	= 1.0; // step act
	L2	= 0.0; // step act
	eps = 10.0;// ramp
	teta = 0.0;
	OK = 0.01;
	type = 4;
}
/*******************************************************************************/
int MLP::run(double *GIR, bool TEACH, int GERCEK){
	/*
		 nKAT adet katmana sahip
			i. katmanında;
				nGIR[i] adet girişe sahip
				nCIK[i] adet çıkışa sahip
				IO[ i ][k] i. katmanın k. girişi
				IO[i+1][j] i. katmanın j. çıkışı
				W[i][j][k] i. katmanda j. çıkışa gelen k. girişin ağırlığı
				double MLP::act(double net) aktivasyon
			fonksiyonu olmak üzere
		çok katmanlı yapay sinir ağında
		çıkışların hesaplanması
		(ileri yayılım)
	*/
	for (int k=0; k<nGIR[0]-1; k++) { IO[0][k] = GIR[k];}
	for (int i=0; i<nKAT; i++) {
		for (int j=0; j<nCIK[i]; j++) {
			double net = 0;
			for (int k=0; k<nGIR[i]; k++) {net+=W[i][j][k]*IO[i][k];}
			IO[i+1][j] = act(net);
		}
	}
	/*
		nKAT adet katmana sahip
			i. katmanında;
				nGIR[i] adet girişe sahip
				nCIK[i] adet çıkışa sahip
				IO[ i ][k] i. katmanın k. girişi
				IO[i+1][j] i. katmanın j. çıkışı
				W[i][j][k] i. katmanda j. çıkışa gelen i. girişin ağırlığı
			olmak üzere
		Eğer eğitim yapılacaksa;
			doğru değer [0 29] arasında bir sayı
			IO[nKAT][k] (nKAT-1). (son) katmanın k. çıkışı [0 1] aralığında
			"!!! ilk katman 0. katman, son katman (nKAT-1)."
		hatanın geri yayılımı
	*/
	if(TEACH==true){
		for (int k=0; k<nCIK[nKAT-1]; k++) {
			double O = (30.0 * IO[nKAT][k]);
				ERR[nKAT-1][k] = double(GERCEK)+0.5-O;
		}
		for (int i=nKAT-2; i>=0; i--) {
			for (int j=0; j<nCIK[i]; j++) {
				ERR[i][j] = 0;
				for (int k=0; k<nCIK[i+1]; k++){ERR[i][j] += W[i+1][k][j]*ERR[i+1][k];}
			}
		}
		/*
			nKAT adet katmana sahip
			i. katmanında;
				nGIR[i] adet girişe sahip
				nCIK[i] adet çıkışa sahip
				IO[ i ][k] i. katmanın k. girişi
				IO[i+1][j] i. katmanın j. çıkışı
				W[i][j][k] i. katmanda j. çıkışa gelen i. girişin ağırlığı
				ERR[i][j] i. katmanın j. çıkışının hatası
			OK öğrenme katsayısı  olmak üzere
			yol ağırlıklarının güncellenmesi
		*/
		for (int i=0; i<nKAT; i++) {
			for (int j=0; j<nCIK[i]; j++) {
				for (int k=0; k<nGIR[i]; k++)
				{W[i][j][k] += OK*act_der(IO[i+1][j])*ERR[i][j]*IO[i][k];}
			}
		}
	}
	/***************************************************************************/
	int R=int(30 *IO[nKAT][0]);
	if (R < 0) {R = 0;}
	if (R > 29) { R = 29; }
	return R;
}
/*******************************************************************************/
double MLP::act(double net) {
	double H;
	switch (type) {
	case 1: return L * (net - teta);							// lineer
	case 2: if (net > teta) {return L1;}else{return L2;}		// step
	case 3:														// ramp		
		
			 if ((net-teta)> 1){H=  1;}
		else if ((net-teta)<-1){H= -1;}
		else					 {H= (net-teta);}
			 return H / 2 + 0.5;
	case 4: return 1/(1+exp(-L*(net-teta)));					// sigmoid	
	case 5:														// hyperbolic
		H= exp(L*(net-teta));
		return ((H-1/H)/(H+1/H)+1)*0.5;
	case 6: return exp(-(net-teta)*(net-teta)/(S*S));		// gaussian	
	default: return 0;
	}
}
/*******************************************************************************/
double MLP::act_der(double CIK,double net) {
	switch (type) {
	case 1: return L;					// lineer
	case 2: return 1;					// step
	case 3: return 1;					// ramp		
	case 4: return CIK * (1 - CIK);		// sigmoid	
	case 5: return (1 - CIK * CIK);		// hyperbolic
	case 6: return -net * CIK / (S*S);	// gaussian
	default: return 1;
	}
}
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
LVQ::~LVQ() {
	D=	Free_Matrix_1D(D);
	W=	Free_Matrix_2D(W);
}
/*******************************************************************************/
/*
Learning Vector Quantization (LVQ) ağında
	nCIK grup sayısı
	nPSS herbir gruptaki çıkış sayısı
	nH toplam çıkış hücresi sayısı
	W[i][j] i. hücreye gelen j. girişin ağırlığı
	D[i] i. hücrenin girişleri ile yol ağırlıklarının arasındaki 
	farkların karesinin toplamı
olmak üzere hafıza belirleme ve
W[i][j] i. hücrenin j. giriş ağırlığını 
[-1 1] aralığında rastlantısal değerlerle yapılandırma
*/
LVQ::LVQ(int _nGIR, int _nCIK) {
	OK	= 0.001;
	nGIR= _nGIR;
	nCIK= _nCIK;
	nPSS= 6;
	nH	= nCIK * nPSS;
	D=NULL; D=	Init_Matrix_1D<double>(nH);
	W=NULL; W=	Init_Matrix_2D<double>(nH,nGIR);
	for (int i=0; i<nH; i++) {
		for (int j=0; j<nGIR; j++)
		{ W[i][j] = 2.0*double(rand())/double(RAND_MAX)-1; }
	}
}
/*******************************************************************************/
int	LVQ::run(double *GIR, bool TEACH,int GERCEK) {

	/*
		Learning Vector Quantization (LVQ) ağında
			nCIK grup sayısı
			nPSS herbir gruptaki çıkış sayısı
			nH toplam çıkış hücresi sayısı
			GIR[j] giriş vektörünün j. elemanı
			W[i][j] i. hücreye gelen j. girişin ağırlığı
			D[i] i. hücrenin girişleri ile yol ağırlıklarının arasındaki 
			farkların karesinin toplamı
			ind1 en küçük farka sahip hücrenin toplam hücre içindeki sırası
		olmak üzere
		en küçük farka sahip i. hücrenin değerinin ve kaçıncı hücre olduğunun belirlenmesi.
		"min(D[i]) ve ind1"
		ind1. hücrenin kaçıncı gruba ait olduğunun belirlenerek 
		GIR[] vektörünün hangi grupta olduğu kararının verilmesi
	*/
	int		SONUC;
	int		ind1 = 0;
	double	M1 = 1E9;
	double	S;
	for (int i=0; i<nH; i++) {
		D[i] = 0;
		for (int j=0; j<nGIR; j++)
		{D[i] += (GIR[j] - W[i][j])*(GIR[j] - W[i][j]);}
		if (M1 > D[i]) { M1 = D[i]; ind1 = i;}
	}
	SONUC = int(ind1 / nPSS);
	/*
		Learning Vector Quantization (LVQ) ağında eğitim
			ind1 kazanan çıkış hücresi sırası
			"en küçük farka sahip hücrenin toplam hücre içindeki sırası"
			GERÇEK GIR[] vektörünün ait olması gereken grup
			SONUÇ GIR[] vektörünün ait olduğu hesaplanan grup
			"SONUÇ kazanan hücre tarafından belirlenir"			
		olmak üzere
		ilgili yol ağırlığının güncellenmesi
	*/
	if (TEACH == true) {
		if (SONUC == GERCEK) { S = 1.0; }else { S = -1.0; }
		for (int j = 0; j < nGIR; j++)
		{W[ind1][j] += OK * S*(GIR[j] - W[ind1][j]);}
		OK *= 0.999;
	}
	return SONUC;
}
/*******************************************************************************/
LV2::~LV2() {
	D=	Free_Matrix_1D(D);
	W=	Free_Matrix_2D(W);
}
/*******************************************************************************/
LV2::LV2(int _nGIR, int _nCIK) {
	OK	= 0.001;
	nGIR= _nGIR;
	nCIK= _nCIK;
	nPSS= 6;
	nH	= nCIK * nPSS;
	D=NULL; D=	Init_Matrix_1D<double>(nH);
	W=NULL; W=	Init_Matrix_2D<double>(nH,nGIR);
	for (int i=0; i<nH; i++) {
		for (int j=0; j<nGIR; j++) 
		{ W[i][j] = 2.0*double(rand())/double(RAND_MAX)-1; }
	}
}
/*******************************************************************************/
int	LV2::run(double *GIR, bool TEACH, int GERCEK) {
	int		SONUC1;
	int		SONUC2;
	int		ind1=0;
	int		ind2=0;
	double	M1  = 1E9;
	double	M2  = 1E9;
	double	S;
	for (int i=0; i<nH; i++) {
		D[i] = 0;
		for (int j=0; j<nGIR; j++)
		{D[i] += (GIR[j] - W[i][j])*(GIR[j] - W[i][j]);}
		if (M1 > D[i]) { M1 = D[i]; ind1 = i; }
	}
	SONUC1 = int(ind1 / nPSS);
	if (TEACH == true) {
		for (int i = 0; i < nH; i++) {
			if (M2 > D[i] && i != ind1) { M2 = D[i]; ind2 = i; }
		}
		SONUC2 = int(ind2 / nPSS);
		/**************************************************/
		if (SONUC1 != SONUC2 && SONUC2 == GERCEK) {
			for (int j = 0; j < nGIR; j++) {
				W[ind1][j] -= OK *  (GIR[j] - W[ind1][j]);
				W[ind2][j] += OK *  (GIR[j] - W[ind2][j]);
			}
		}
		/*********************************************************/
		OK *= 0.999;
	}
	return SONUC1;
}
/*******************************************************************************/
LVX::~LVX() {
	D=	Free_Matrix_1D(D);
	W=	Free_Matrix_2D(W);
}
/*******************************************************************************/
LVX::LVX(int _nGIR, int _nCIK) {
	OK	= 0.001;
	nGIR= _nGIR;
	nCIK= _nCIK;
	nPSS= 6;
	nH	= nCIK * nPSS;
	D=NULL; D=	Init_Matrix_1D<double>(nH);
	W=NULL; W=	Init_Matrix_2D<double>(nH,nGIR);
	for (int i=0; i<nH; i++) {
		for (int j=0; j<nGIR; j++) 
		{ W[i][j] = 2.0*double(rand())/double(RAND_MAX)-1; }
	}
}
/*******************************************************************************/
int	LVX::run(double *GIR, bool TEACH, int GERCEK) {
	int		SONUC;
	int		ind1=0;
	int		ind2=0;
	int		ind3=0;
	double	M1  = 1E9;
	double	M2  = 1E9;
	double	M3  = 1E9;
	double	S;
	for (int i=0; i<nH; i++) {
		D[i] = 0;
		for (int j=0; j<nGIR; j++)
		{D[i] += (GIR[j] - W[i][j])*(GIR[j] - W[i][j]);}
		if (M1 > D[i]) { M1 = D[i]; ind1 = i; }
	}
	SONUC = int(ind1 / nPSS);
	if (TEACH == true) {
		for (int i = 0; i < nH; i++) {
			if (M2 > D[i] && i != ind1) { M2 = D[i]; ind2 = i; }
		}
		if (SONUC == GERCEK) { S = 1.0; }else { S = -1.0; }
		for (int j = 0; j < nGIR; j++){
			W[ind1][j] += OK * S*(GIR[j] - W[ind1][j]);
			W[ind2][j] -= OK * S*(GIR[j] - W[ind2][j]);
		}
		//------------------------------------------------------------------------
		int ilk = GERCEK*nPSS;
		int son = ilk + nPSS;
		ind3 = ilk;
		M3 = D[ilk];		
		for (int i = ilk; i < son; i++) {
			if (M3 > D[i]) { M3 = D[i]; ind3 = i; }
		}
		if (ind1 != ind3) {
			for (int j = 0; j < nGIR; j++)
			{W[ind3][j] += OK * (GIR[j] - W[ind3][j]);}
		}
//------------------------------------------------------------------------
		OK *= 0.999;
	}
	return SONUC;
}
/*******************************************************************************/
ART1::~ART1() {	Mem_DeAlloc();}
/*******************************************************************************/
ART1::ART1(int _nGIR, int _nCIK) {
	nGIR = _nGIR;
	nCIK = _nCIK;
	rho = 0.7; // benzerlik katsayısı %
	Mem_Alloc();
	Weights_Init();
	A.clear();
}
/*******************************************************************************/
void ART1::Mem_Alloc() {
	Y = new double[nCIK];
	Bji = new double* [nCIK];	for (int j = 0; j < nCIK; j++) { Bji[j] = new double[nGIR]; }
	Tij = new double* [nGIR];	for (int i = 0; i < nGIR; i++) { Tij[i] = new double[nCIK]; }
}
/*******************************************************************************/
void ART1::Mem_DeAlloc() {
	delete Y;
	for (int j = 0; j < nCIK; j++) { delete[]Bji[j]; }	delete[]Bji;
	for (int i = 0; i < nGIR; i++) { delete[]Tij[i]; }	delete[]Tij;
}
/*******************************************************************************/
int	ART1::run(double* _GIR, bool TEACH) {
	int RET_VAL=0;
	bool UPDATED = false;
	std::vector<int>TABU;
	TABU.clear();	
	ibestF2 = 0;
	/**************************************************************/
	if (A.size() > 0) {
		/* 2 */
		update_F2(_GIR);		
		/* 3 Kazanan (maksimum) çıkışın bulunması */		
		do {
			ibestF2 = best_F2(TABU);
			if (ibestF2 < 0 || ibestF2 >= A.size()) { break; }
			/* Uyuşum testi. Kazanan elemanın ağırlık vektörü Z [i][ibestF2]*/
			/* iki vektör birbirinin benzeri ise */
			if (update_R(_GIR) > rho) {
				if (TEACH) {update_WEIGHTS(ibestF2, _GIR);}
				UPDATED = true;
				RET_VAL = ibestF2;
			}
			else {
				TABU.push_back(ibestF2);	
			}
		} while (!UPDATED && TABU.size() < A.size());
	}
	/* 4 */
	if (UPDATED==false) {		
		if (TEACH) {
			if (A.size() >= nCIK) { return -1; }
			A.push_back(A.size());
			update_WEIGHTS(A.back(), _GIR);
		}
		RET_VAL = A.back();
	}	
	return RET_VAL;
}
/*******************************************************************************/
void ART1::update_F2(double* GIR) {
	for (int k = 0; k < A.size(); k++) {
		int j = A[k];
		Y[j] = 0;
		for (int i = 0; i < nGIR; i++) {
			Y[j] += (Bji[j][i] * GIR[i]);
		}
	}
}
/*******************************************************************************/
void ART1::Input_Show(double *_GIR) {
	std::cout << "INPUT PATTERNS: ";
	for (int i = 0; i < nGIR; i++) { std::cout << _GIR[i] << " "; }
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;
}
/*******************************************************************************/
int ART1::best_F2(std::vector<int>TABU) {
	double	ymax = 0;
	int _ibestF2 = -1;
	for (int k = 0; k < A.size(); k++) {
		int j = A[k];
		bool YASAK = false;
		for (int r = 0; r < TABU.size(); r++) {
			if (j == TABU[r]) { YASAK = true; break; }
		}
		if (!YASAK) {
			if (ymax < Y[j]) {
				ymax = Y[j];
				_ibestF2 = j;
			}
		}
	}
	return _ibestF2;
}
/*******************************************************************************/
double ART1::update_R(double *_GIR) {
	double S1 = 0;
	double S2 = 0;
	for (int i = 0; i < nGIR; i++) {
		S1 += _GIR[i];
		S2 += Tij[i][ibestF2] * _GIR[i];
	}
	return S2 / S1;
}
/*******************************************************************************/
void ART1::Weights_Init() {
	for (int j = 0; j < nCIK; j++) { for (int i = 0; i < nGIR; i++) { Bji[j][i] = 1 / (1 + nGIR); } }
	for (int i = 0; i < nGIR; i++) { for (int j = 0; j < nCIK; j++) { Tij[i][j] = 1; } }
}
/*******************************************************************************/
void ART1::update_WEIGHTS(int j, double* GIR) {
	double sum = 0;
	for (int i = 0; i < nGIR; i++) { sum += Tij[i][j] * GIR[i]; }
	for (int i = 0; i < nGIR; i++) {
		Bji[j][i] = (Tij[i][j] * GIR[i]) / (0.5 + sum);
		Tij[i][j] = (Tij[i][j] * GIR[i]);
	}
}
/*******************************************************************************/
void ART1::remove_CLUSTER(int ind,int N) {
	for (int j = ind; j < N-1; j++) {
		for (int i = 0; i < nGIR; i++) {
			Bji[j][i] = Bji[j+1][i];
			Tij[i][j] = Tij[i][j+1];
		}
	}
	for (int i = 0; i < nGIR; i++) {
		Bji[N-1][i] = 1 / (1 + nGIR);
		Tij[i][N-1] = 1;
	}
}
/*******************************************************************************/
void ART1::Weights_Show() {
	std::cout << std::endl;
	std::cout << "BU WEIGHTS:" << std::endl;
	for (int j = 0; j < A.size(); j++) { for (int i = 0; i < nGIR; i++) { std::cout << Bji[j][i] << "\t"; } std::cout << std::endl; }
	std::cout << std::endl;
	std::cout << "TD WEIGHTS:" << std::endl;
	for (int i = 0; i < nGIR; i++) { for (int j = 0; j < A.size(); j++) { std::cout << Tij[i][j] << "\t"; } std::cout << std::endl; }
	std::cout << "-----------------------------------------" << std::endl;
}
/*******************************************************************************/
void ART1::Weights_Print(std::string F_NAME) {
	std::fstream DOSYA;
	DOSYA.open(F_NAME, std::ios::out);
	DOSYA.precision(3);
	DOSYA << "\t\t Bottom-up weights:\n\n";
	for (int j = 0; j < nCIK; j++) { for (int i = 0; i < nGIR; i++) { DOSYA << Bji[j][i] << " "; } DOSYA << "\n"; }
	DOSYA << "\n\t\t Top-down weights:\n\n";
	for (int i = 0; i < nGIR; i++) { for (int j = 0; j < nCIK; j++) { DOSYA << Tij[i][j] << " "; } DOSYA << "\n"; }
	DOSYA << "\n";
	DOSYA.close();
}
/*******************************************************************************/
