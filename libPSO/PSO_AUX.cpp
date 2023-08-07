namespace MY {typedef unsigned int uint;}
#include <iostream>
#include <cmath>
#include "PSO_AUX.h"

bool TEKRAR(double** HALK, double* B,
	const MY::uint& ilk,
	const MY::uint& son,
	const MY::uint& inPUB,
	const MY::uint& inDIM) {
	double EPS = 1E-20;
	if (son > inPUB || son <= ilk) { return false; }
	for (MY::uint i = ilk; i < son; i++) {
		bool	VAR = true;
		for (MY::uint j = 0; j < inDIM; j++) {	// herhangi bir [0,nDIM) aralığındaki eleman
			double FARK=(B[j] - HALK[i][j]);
			if (fabs(FARK) > EPS){VAR = false; break;}
		}
		if (VAR == true) {return true;}
	}
	return false;
}
/**********************************************************************************************/
bool TEKRAR_FULL(double** HALK,
	const MY::uint& son,
	const MY::uint& inDIM) {

	double EPS = 1E-20;
	int CNT=0;
	for (MY::uint i1 = 1; i1 < son; i1++) {
		for (MY::uint i2 = i1-1; i2 >=0; i2--) {
			bool	VAR = true;
			for (MY::uint j = 0; j < inDIM; j++) {
				if (fabs(HALK[i1][j] - HALK[i2][j]) > EPS){VAR = false; break;}
			}
			if (VAR == true) { CNT++; /*return true;*/ break;}
		}
	}
	if(CNT!=0){
		std::cout<<"AYNI BIREY SAYISI: "<<CNT<<" of "<<son<<"\n";
		return true;
	}
	return false;
}
/**********************************************************************************************/
