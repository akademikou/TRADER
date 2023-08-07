/**********************************************************************************************/
#pragma once
#ifndef CLASS_ANN_H
#define CLASS_ANN_H
#include "CLASS_KON.h"
#define CLASS_ANN_API __declspec(dllexport)
/**********************************************************************************************/

#include <iostream>
#include <vector>
/*******************************************************************************/
class MDL{
public:
protected:
	double	*CIK;
	double	**W;
	int		nGIR;
	int		nCIK;
	double	OK;
public:
	virtual ~MDL();
	MDL(int _nGIR,int _nCIK);
	int	run(double *GIR, bool TEACH, int GERCEK);
};
/*******************************************************************************/
/*******************************************************************************/
class MLP {
public:
protected:
	int		nKAT;
	int		*nGIR;
	int		*nCIK;
	int		*YAPI;

	double **ERR;
	double **IO;
	double ***W;

	double	S;
	double	L;
	double	L1;
	double	L2;
	double	eps;
	double	teta;
	double	OK;
	int		type;
public:
	virtual ~MLP();
	MLP(int _nGIR,int _nCIK);
	int	run(double *GIR, bool TEACH, int GERCEK);
protected:
	double act(double net);
	double act_der(double CIK, double net = 1);
};
/*******************************************************************************/
/*******************************************************************************/
class LVQ {
public:
protected:
	int		nGIR;
	int		nCIK;
	int		nPSS;
	int		nH;
	double	OK;
	double	*D;
	double	**W;
public:
	virtual ~LVQ();
	LVQ(int _nGIR,int _nCIK);
	int	run(double *GIR, bool TEACH, int GERCEK);
};
/*******************************************************************************/
class LV2{
public:
protected:
	int		nGIR;
	int		nCIK;
	int		nPSS;
	int		nH;
	double	OK;
	double	*D;
	double	**W;
public:
	virtual ~LV2();
	LV2(int _nGIR, int _nCIK);
	int	run(double *GIR, bool TEACH, int GERCEK);
};
/*******************************************************************************/
class LVX{
public:
protected:
	int		nGIR;
	int		nCIK;
	int		nPSS;
	int		nH;
	double	OK;
	double	*D;
	double	**W;
public:
	virtual ~LVX();
	LVX(int _nGIR, int _nCIK);
	int	run(double *GIR, bool TEACH, int GERCEK);
};
/*******************************************************************************/
class ART1 {
public:
protected:
	int		nGIR;
	int		nCIK;
	double	OK;
	double	rho;			// benzerlik katsayısı	
	int		ibestF2;

	double* Y;
	double** Tij;			// yukarıdan aşağı ağırlıklar nGIR x nCIK boyutlu
	double** Bji;			// aşağıdan yukarı ağırlıklar nCIK x nGIR boyutlu
	std::vector<int>A;
public:
	virtual ~ART1();
	ART1(int _nGIR, int _nCIK);
	int	run(double* GIR, bool TEACH);
	void Input_Show(double* _GIR);
	void Weights_Show();
	void Weights_Print(std::string F_NAME);
protected:
	void Mem_Alloc();
	void Mem_DeAlloc();
	void update_F2(double* GIR);
	int best_F2(std::vector<int>TABU);
	double update_R(double* GIR);
	void update_WEIGHTS(int j, double *GIR);
	void remove_CLUSTER(int j, int N);	
	void Weights_Init();
};
/*******************************************************************************/
#endif
/**********************************************************************************************/