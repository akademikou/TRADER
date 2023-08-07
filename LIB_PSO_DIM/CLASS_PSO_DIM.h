#ifndef PATH
	#if defined(_MSC_VER)    //  Microsoft
		#define PATH "C:/Users/akade/Documents/R_PROJ_LVQ/"
//		#define PATH ""
	#elif defined(__GNUC__)   //  GCC
		#define PATH "/root/R_PROJ_LVQ/"
	#endif
#endif
/***********************************************************************************/
#pragma once
#ifndef CLASS_PSO_DIM_H
#define CLASS_PSO_DIM_H

#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT_PSO_DIM __declspec(dllexport)
    #define IMPORT_PSO_DIM __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define EXPORT_PSO __attribute__((visibility("default")))
    #define IMPORT_PSO
#else
    //  do EXPORT_PSO and hope for the best?
    #define EXPORT_PSO
    #define IMPORT_PSO
    #pragma warning Unknown dynamic link import/export semantics.
#endif
#include "xoshiro.h"
using namespace XoshiroCpp;
XoshiroCpp::Xoshiro256PlusPlus *mt_A_PSO_DIM;
std::uniform_real_distribution<double> dist_R_PSO_DIM;
std::uniform_int_distribution<int>	   dist_I_PSO_DIM;
const double MAX_VAL_PSO_DIM = 1E20;
const double MIN_VAL_PSO_DIM = 1E-5;
/**********************************************************************************************/
class EXPORT_PSO_DIM PSO_DIM {
protected:
	double *sum;
	double *sq_sum;
//	std::mt19937_64		mt_A_PSO;
	MY::uint nProcessors;
	MY::uint LDIM;
	MY::uint aDIM;
	double mut_rate; // mutasyon oranı
	double cro_rate; // çaprazlama oranı
	MY::uint iterMAX;
	MY::uint nLOC; // Local best için komşu sayısı
	MY::uint * nTH_LIM;
	MY::uint * nTH_BSL;
	MY::uint * nTH_BTS;
	MY::uint nDIM;
	MY::uint nPUB;

	double* aSNR; // aranan değişkenlerin alt sınırı
	double* uSNR; // aranan değişkenlerin üst sınırı
	double*	araSNR;	// aranan değişkenlerin aralık farkı
	double** P_MEAN;
	double** G_BEST;

	double** VEL;
	double** Child;
	double** dPUB; // topluluk anlık değer
	double** P_BEST; // topluluk bireysel en iyi değerler
	double** L_BEST; // topluluk bireysel en iyi değerler
	KON* func;
public:
	double RUN(double* EN_IYI);
	virtual ~PSO_DIM();
	PSO_DIM(KON* infunc, const MY::uint& nI);
	void SET_BEST(double* EN_IYI);
protected:
	void init_HAFIZA();
	void read_OPT();
	void init_POP(const bool& use_gaussian);
	void inject_POP();	
	void URET();	
	void UPDATE_LG_BEST();
	bool CRITERIA(const bool verbose);
};
#endif
/**********************************************************************************************/