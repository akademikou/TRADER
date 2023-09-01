/***********************************************************************************/
#pragma once
#ifndef CLASS_PSO_H
#define CLASS_PSO_H

#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT_PSO __declspec(dllexport)
    #define IMPORT_PSO __declspec(dllimport)
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
XoshiroCpp::Xoshiro256PlusPlus *mt_A_PSO;
std::uniform_int_distribution<int> dist_I_PSO;
const double MAX_VAL_PSO = 1E20;
/**********************************************************************************************/
class EXPORT_PSO PSO {
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
	MY::uint *nTH_LIM;
	MY::uint *nTH_BSL;
	MY::uint *nTH_BTS;
	MY::uint nDIM;
	MY::uint nPUB;

	double *aSNR; // aranan değişkenlerin alt sınırı
	double *uSNR; // aranan değişkenlerin üst sınırı
	double *araSNR;	// aranan değişkenlerin aralık farkı

	double **P_MEAN;
	double **G_BEST;
	double **G_WRST;

	double **VEL;
	double **dPUB; // topluluk anlık değer
	double **P_BEST; // topluluk bireysel en iyi değerler
	double **L_BEST; // topluluk bireysel en iyi değerler
	double **tmpP; // topluluk bireysel en iyi değerler
	KON *func;
	MY::uint nTAM;
	MY::uint nCRITER;
public:
	double RUN(double *EN_IYI);
	virtual ~PSO();
	PSO(KON *infunc, const MY::uint &nI);
	void SET_BEST(double *EN_IYI);
protected:
	void init_HAFIZA();
	void read_OPT();
	void init_POP(const bool &use_gaussian);
	void inject_POP();
	void URET();
	bool CRITERIA(const bool verbose);
	inline void ALLERT(const int RENK);
	inline bool COMPARE_UPDATE_B(double *_In1, const double *_In2,
			const bool UPDATE);
	inline bool COMPARE_UPDATE_W(double *_In1, const double *_In2,
			const bool UPDATE);
	inline bool NOTIFY(double *_In1, const MY::uint &_id, const MY::uint &_i,
			const bool verbose);
};
#endif
/**********************************************************************************************/
