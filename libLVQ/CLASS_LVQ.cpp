#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <cmath>
#include <chrono>
#include "MATRIX.h"
#include "CLASS_LVQ.h"

/*******************************************************************************/
/*******************************************************************************/
LVQ::~LVQ() {
	D = Free_Matrix_1D(D);
	W = Free_Matrix_2D(W);
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
LVQ::LVQ(int _nGIR, int _nCIK, int _nPSS, const double _OK) {
	dist_R_LVQ.param(std::uniform_real_distribution<double>::param_type{-100,100});
	mt_A_LVQ.seed(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())) ;

	OK = _OK;
	nGIR = _nGIR;
	nCIK = _nCIK;
	nPSS = _nPSS;
	nH = nCIK * nPSS;
	D = NULL; D = Init_Matrix_1D<double>(nH);
	W = NULL; W = Init_Matrix_2D<double>(nH, nGIR);
	for (int i = 0; i < nH; i++) {
		D[i]=0;
		for (int j = 0; j < nGIR; j++){W[i][j] = dist_R_LVQ(mt_A_LVQ);}
	}
}
/*******************************************************************************/
void LVQ::set_W(double** _W) {
	for (int i = 0; i < nH; i++) {
		for (int j = 0; j < nGIR; j++) { W[i][j] = _W[i][j]; }
	}
}
/*******************************************************************************/
void LVQ::get_W(double** _W) {
	for (int i = 0; i < nH; i++) {
		for (int j = 0; j < nGIR; j++) { _W[i][j] = W[i][j]; }
	}
}
/*******************************************************************************/
void LVQ::yaz_W(std::string name) {
	std::fstream DOSYA;
	DOSYA.open(name.c_str(), std::ios::out);
	DOSYA << std::setprecision(6);
	if(!DOSYA.is_open()){std::cout<<name.c_str()<<" not opened\n";}

	if(!DOSYA.is_open()){std::cout<<PATH+name<<" not opened\n";}
	for (int i = 0; i < nH; i++) {
		for (int j = 0; j < (nGIR-1); j++){	DOSYA<<W[i][j]<<"\t";}
		DOSYA<<W[i][(nGIR-1)]<<"\n";
	}
	DOSYA.close();
}
/*******************************************************************************/
/*	Learning Vector Quantization (LVQ) ağında
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
		Learning Vector Quantization (LVQ) ağında eğitim
		ind1 kazanan çıkış hücresi sırası
		"en küçük farka sahip hücrenin toplam hücre içindeki sırası"
		GERÇEK GIR[] vektörünün ait olması gereken grup
		SONUÇ GIR[] vektörünün ait olduğu hesaplanan grup
		"SONUÇ kazanan hücre tarafından belirlenir"
	olmak üzere
	ilgili yol ağırlığının güncellenmesi
*/
int	LVQ::run(const double* GIR, const bool& TEACH, const int& GERCEK) {
	int		SONUC1;
	int		SONUC2;
	int		ind1 = 0;
	int		ind2 = 0;
	int		ind3 = 0;
	int		ilk=0,son=0;

	for (int i = 0; i < nH; i++) {
		D[i] = 0;
		for (int j = 0; j < nGIR; j++) {
			double err=(GIR[j]-W[i][j]);
			D[i] += (err * err);
//			D[i] += fabs(err);
		}
	}
	for (int i = 1; i < nH; i++) { if (D[i]<D[ind1]) { ind1 = i;}}
	SONUC1 = int(ind1 / nPSS);
	/*****************************************************************/
	if (TEACH == true) {
		if(ind1==0){ind2=1;}else{ind2=0;}
		for (int i = 0; i < nH; i++) { if (D[i] < D[ind2] && i != ind1) {ind2 = i; } }
		SONUC2 = int(ind2 / nPSS);

		ilk = GERCEK * nPSS;
		son = ilk + nPSS;
		ind3 = ilk;
		for (int i = ilk; i < son; i++) { if (D[i]< D[ind3]) { ind3 = i; } }

		double S1=0,S2=0,S3=0;
		double eps=0.2;
		double dc1=sqrt(D[ind1]);
		double dc2=sqrt(D[ind2]);

		bool T1 =SONUC1!=SONUC2;
		bool T2a=SONUC1==GERCEK;
		bool T2b=SONUC2==GERCEK;
	//	bool T3 =(dc1/dc2) > (1-eps) && (dc2/dc1) < (1+eps);
		bool T3 =(dc1/dc2) > (1-eps)*(1+eps); // LVQ3 için

		if( T1 && T2a && T3){S1 =   1;S2 =  -1;}	//   2.1 3
		if( T1 && T2b && T3){S1 =  -1;S2 =   1;}	// 2 2.1 3
		if(!T1 && T2a && T3){S1 = 0.4;S2 = 0.4;}	//       3
		/**************************************************/
		// KLASIK
//		if (T2a) {S1 = 1;}else{S1 = -1;}
//		if (T2b) {S2 = 1;}else{S2 = -1;}
//		if (!T1) {S2 = 0;}
		if (T2a || T2b) { S3 = 0; }else {S3 = 1;}


		/**************************************************/
		for (int j = 0; j < nGIR; j++) {
			W[ind1][j] += (OK * S1*(GIR[j]-W[ind1][j]));
			W[ind2][j] += (OK * S2*(GIR[j]-W[ind2][j]));
			W[ind3][j] += (OK * S3*(GIR[j]-W[ind3][j]));
		}
		/**************************************************/
		OK *= 0.999999;
	}
	return SONUC1;
}
/*******************************************************************************/
//void LVQ::TRAIN_TEST(const double** &_TEST_DATA, const std::vector<int>&_GERCEK,
//	const int _ILK,const int _SON, const bool &TRAIN) {
void LVQ::TRAIN_TEST(const std::vector<std::vector<double>>_TEST_DATA,
	const std::vector<int>&_GERCEK,
	const int _ILK,const int _SON, const bool &TRAIN) {
	dist_I_LVQ.param(std::uniform_int_distribution <int>::param_type{ _ILK,_SON - 1 });
	mt_A_LVQ.seed(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())) ;

	double* G_YSA=new double[nGIR];

	int M_lvq = 0,int_D = 0;
	int R[3][3]={{0,0,0},{0,0,0},{0,0,0}};


	if(TRAIN){
		for (int iter = 0; iter < 1e7; iter++) {
			int ind = dist_I_LVQ(mt_A_LVQ);
	//		ind=_ILK+iter%(_SON-_ILK);
			for (int j = 0; j < nGIR; j++) { G_YSA[j] = _TEST_DATA[ind][j];}
			if(_GERCEK[ind]== -1){int_D=0;}
			else{
				if(nCIK==2){int_D = 1;}
				if(nCIK==3){int_D = (_GERCEK[ind] == 0) ? 1:2;}
			}
			M_lvq = this->run(G_YSA, TRAIN, int_D);
			R[int_D][M_lvq]++;
		}
	}else{
		for (int ind= _ILK ; ind < _SON; ind++) {
			for (int j = 0; j < nGIR; j++) { G_YSA[j] = _TEST_DATA[ind][j];}
			if(_GERCEK[ind]== -1){int_D=0;}
			else{
				if(nCIK==2){int_D = 1;}
				if(nCIK==3){int_D = (_GERCEK[ind] == 0) ? 1:2;}
			}
			M_lvq = this->run(G_YSA, TRAIN, int_D);
			R[int_D][M_lvq]++;
		}
	}
	std::cout << "______________________________________" << std::endl;
	for(int i=0;i<3;i++){
		std::cout	<< i<<"0: "<<R[i][0]<<" ("<< double(R[i][0]) / double(R[i][0] + R[i][1]+ R[i][2]) <<")\t"
					<< i<<"1: "<<R[i][1]<<" ("<< double(R[i][1]) / double(R[i][0] + R[i][1]+ R[i][2]) <<")\t"
					<< i<<"2: "<<R[i][2]<<" ("<< double(R[i][2]) / double(R[i][0] + R[i][1]+ R[i][2]) <<")\n";
	}
	std::cout << "______________________________________" << std::endl;
	delete []G_YSA;
}
