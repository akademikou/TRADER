/***********************************************************************************/
#pragma once
#ifndef CLASS_LVQ_H
#define CLASS_LVQ_H

#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT_LVQ __declspec(dllexport)
    #define IMPORT_LVQ __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT_LVQ __attribute__((visibility("default")))
#define IMPORT_LVQ
#else
    //  do EXPORT_LVQ and hope for the best?
    #define EXPORT_LVQ
    #define IMPORT_LVQ
    #pragma warning Unknown dynamic link import/export semantics.
#endif
/**********************************************************************************************/
std::mt19937_64 mt_A_LVQ;
std::random_device rd_A_LVQ;
std::uniform_real_distribution<double> dist_R_LVQ;
std::uniform_int_distribution<int> dist_I_LVQ;
const double MAX_VAL = 1E20;
const double MIN_VAL = 1E-5;

class EXPORT_LVQ LVQ {
public:
protected:
	int nH;
	int nGIR;
	int nCIK;
	int nPSS;
	double OK;
	double *D;
	double **W;
public:
	virtual ~LVQ();
	LVQ(const int _nGIR, const int _nCIK, const int _nPSS, const double _OK);
	void set_W(double **_W);
	void get_W(double **_W);
	void yaz_W(std::string name);
	int run(const double *GIR, const bool &TEACH, const int &GERCEK);
//	void  TRAIN_TEST(const double** &_TEST_DATA, const std::vector<int>&_GERCEK,
//		const int _ILK,const int _SON, const bool &TRAIN) ;
	void TRAIN_TEST(const std::vector<std::vector<double>> _TEST_DATA,
			const std::vector<int> &_GERCEK, const int _ILK, const int _SON,
			const bool &TRAIN);
};
/*******************************************************************************/
#endif
/**********************************************************************************************/
