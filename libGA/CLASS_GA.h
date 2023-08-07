/**********************************************************************************************/
#pragma once
#ifndef CLASS_GA_H
#define CLASS_GA_H
#include "GA_AUX.h"
#include "CLASS_KON.h"

#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT_GA __declspec(dllexport)
    #define IMPORT_GA __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define EXPORT_GA __attribute__((visibility("default")))
    #define IMPORT_GA
#else
    //  do nothing and hope for the best?
    #define EXPORT_GA
    #define IMPORT_GA
    #pragma warning Unknown dynamic link import/export semantics.
#endif
/**********************************************************************************************/
class EXPORT_GA GA {
protected:
	int nProcessors;
	double		mut_rate; // mutasyon oranı
	double		cro_rate; // çaprazlama oranı
	unsigned int	nB;
	unsigned int	nDIM;	// problem boyutu (aranan sayısı)
	unsigned int	nPOP;	// ebeveyn sayısı (yeni nesil büyüklüğü)
	unsigned int	nCHILD;	// çocuk sayısı (yeni katılacak birey sayısı)
	unsigned int	nPUB;	// topluluk büyüklüğü	(=nPOP+nChild)
	double*			COST;	// uygunluk (fitness) değereleri
	double*			nCOST;	// ters çevrilmiş ve normalize edilmiş uygunluk değerleri için
	double**		dPUB;	// Tüm topluluk GERCEK kodlama
	double*			aSNR;	// aranan değişkenlerin alt sınırı
	double*			uSNR;	// aranan değişkenlerin üst sınırı
	double*			araSNR;	// aranan değişkenlerin aralık farkı

	unsigned int	nElit;	// elit birey sayısı
	unsigned int	iterMAX;// jenerasyon sınırı


	KON* func;
public:
	double RUN(double* EN_IYI);
	virtual ~GA();
	GA(KON* infunc, unsigned int nI, unsigned int nB);
protected:
	void	init_HAFIZA();
	void	init_POP(bool use_gaussian);
	void	URET();
	void	SEC();
	bool	CRITERIA();

};
#endif
/**********************************************************************************************/
