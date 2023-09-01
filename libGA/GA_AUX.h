#pragma once
#ifndef GA_AUX_H
#define GA_AUX_H
void TAKAS(double &Var1, double &Var2);
bool TEKRAR(double **HALK, double *B, const unsigned int &ilk,
		const unsigned int &son, const unsigned int &nPUB,
		const unsigned int &nDIM);
bool TEKRAR_FULL(double **HALK, const unsigned int &son,
		const unsigned int &inDIM);
void SIRALA(unsigned int num, unsigned int dim, double *L1, double *L2,
		double **L3);
#endif
/**********************************************************************************************/
