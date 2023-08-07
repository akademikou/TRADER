#ifndef PATH
	#if defined(_MSC_VER)    //  Microsoft
		#define PATH "C:/Users/akade/Documents/R_PROJ_LVQ/"
//		#define PATH ""
	#elif defined(__GNUC__)   //  GCC
		#define PATH "/root/R_PROJ_LVQ/"
	#endif
#endif
/**********************************************************************************************/
#pragma once
#ifndef CLASS_FLC_H
#define CLASS_FLC_H

#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT_FLC __declspec(dllexport)
    #define IMPORT_FLC __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define EXPORT_FLC __attribute__((visibility("default")))
    #define IMPORT_FLC
#else
    //  do nothing and hope for the best?
    #define EXPORT_FLC
    #define IMPORT_FLC
    #pragma warning Unknown dynamic link import/export semantics.
#endif
/**********************************************************************************************/
class EXPORT_FLC FLC {
public:
	double **C;			// merkezler
	double **W;
protected:
	unsigned int ITER_MAX;
	unsigned int n;		// ornek sayısı
	unsigned int c;		// merkez sayısı
	unsigned int dim;	//
	double m;			// uyelık kuvvetı
	double **X;			// Girisler
	double **D;
	double *J;
	double **SC;
public:
	void RUN();
	virtual ~FLC();
	FLC(unsigned int _n, unsigned int _c,unsigned int _b,double **_X);
	void YAZ(int id);
protected:
	void RUN_F();
	void RUN_K();
	void HESAP_D();
	void HESAP_CF();
	void HESAP_CK();
	void HESAP_WF();
	void HESAP_WK();
	void HESAP_J(unsigned int iter);

};
#endif
/**********************************************************************************************/
