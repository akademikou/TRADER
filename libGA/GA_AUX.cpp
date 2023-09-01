#include <cmath>
#include <iostream>
#include "GA_AUX.h"
/**********************************************************************************************/
// TAKAS fonksiyonu aslında #include <algorithm> başlığında swap(...,...) içinde de vardır
void TAKAS(double &Var1, double &Var2) {
	double Temp = Var1;
	Var1 = Var2;
	Var2 = Temp;
}
/**********************************************************************************************/
bool TEKRAR(double **HALK, double *B, const unsigned int &ilk,
		const unsigned int &son, const unsigned int &nPUB,
		const unsigned int &nDIM) {
//	return false;
	/* B ile girilen nDim uzunluklu dizinin tüm elemanları
	 Halk[inPUB][inDIM] dizisinin içinde var ise (Halk[i=ilk:<son][nDIM]==B[nDIM])
	 TRUE olarak döner
	 */
	double EPS = 1E-20;		// double değişkenler == sb. yerine ~EPS aralığında
							// yaklaşık eşit testi yapılması daha doğrudur.
	if (son > nPUB) {
		return false;
	}			// programın hatalı davranmaması için her ihtimale karşı
	else if (son <= ilk) {
		return false;
	}
	;		// sınır kontrolü
	for (unsigned int i = ilk; i < son; i++) {
		bool VAR = true;
		for (unsigned int j = 0; j < nDIM; j++) {// herhangi bir [0,nDIM) aralığındaki eleman
			if (fabs(B[j] - HALK[i][j]) > EPS)// farklı ise [i][...] farklıdır. Sonrakine, i+1'e git
					{
				VAR = false;
				break;
			}
		}
		if (VAR == true) {
			return true;
		}
	}
	return false;
}
/**********************************************************************************************/
bool TEKRAR_FULL(double **HALK, const unsigned int &son,
		const unsigned int &inDIM) {

	double EPS = 1E-20;
	int CNT = 0;
	for (unsigned int i1 = 1; i1 < son; i1++) {
		for (int i2 = i1 - 1; i2 >= 0; i2--) {
			bool VAR = true;
			for (unsigned int j = 0; j < inDIM; j++) {// herhangi bir [0,nDIM) aralığındaki eleman
				if (fabs(HALK[i1][j] - HALK[i2][j]) > EPS)// farklı ise [i][...] farklıdır. Sonrakine, i+1'e git
						{
					VAR = false;
					;
					break;
				}
			}
			if (VAR == true) {
				CNT++; /*return true;*/
			}
		}
	}
	if (CNT != 0) {
		std::cout << CNT << "\n";
	}
	return false;
}
/**********************************************************************************************/
void SIRALA(unsigned int num, unsigned int dim, double *L1, double *L2,
		double **L3) {
	/*
	 L1 dizisinin [0 num) aralığındaki elemanlarını büyükten küçüğe doğru sıralar.
	 L2 ve L3 [][dim] dizisi ile beraber aynı L1 dizisinin elemanlarıda aynı şekilde yer değiştirilir
	 */
	bool swapped = true;
	unsigned int i1 = 0, i2 = 0, j;
	double temp;
	for (j = 0; (j <= num) && swapped; j++) {
		swapped = false;
		i1 = i2 = 0;
		for (i1 = 0; i1 < (num - 1); i1++) {
			i2 = i1 + 1;
			if (L1[i2] > L1[i1]) {
				temp = L1[i1];
				L1[i1] = L1[i2];
				L1[i2] = temp;
				temp = L2[i1];
				L2[i1] = L2[i2];
				L2[i2] = temp;
				for (unsigned int k = 0; k < dim; k++) {
					temp = L3[i1][k];
					L3[i1][k] = L3[i2][k];
					L3[i2][k] = temp;
				}
				swapped = true;
			}
		}
	}
}
/**********************************************************************************************/
