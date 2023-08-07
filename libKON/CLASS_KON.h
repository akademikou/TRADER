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
#ifndef CLASS_KON_H
	#define CLASS_KON_H

#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT_KON __declspec(dllexport)
    #define IMPORT_KON __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define EXPORT_KON __attribute__((visibility("default")))
    #define IMPORT_KON
#else
    //  do nothing and hope for the best?
    #define EXPORT_KON
    #define IMPORT_KON
    #pragma warning Unknown dynamic link import/export semantics.
#endif
	#define nTH 8
	/*******************************************************************************/
	class EXPORT_KON KON {
	public:

		MY::uint	nDATA;
		MY::uint	nILK;
		MY::uint	nSON;
		double		**DATA;
		int**		EMIR;
	protected:
		MY::uint	nDIM;
		MY::uint	nINP;
		double**	OHLC;
		double*		tRSI;
		
		double		TxnFees;

		double**	W_RBF;
		double**	C_RBF;
		MY::uint nKAT;
		MY::uint *nGIR;
		MY::uint *nCIK;
		MY::uint ***MAP;

		double ENTRY_PRICE[nTH];
		MY::uint CNT[nTH];
		MY::uint EXT[nTH];
		MY::uint YSK[nTH];


	protected:
//		int KARAR	  (const double* _In, const int &_oEMIR,const MY::uint &_i,const MY::uint &_id,MY::uint &_info);
		int KARAR	  (const double* _In, const int &_oEMIR,const MY::uint &_i,MY::uint &_info);
		void KARAR_MLP(const double* _In,       int &_oEMIR,const MY::uint &_i,double &_net);
		void KARAR_SLP(const double* _In,       int &_oEMIR,const MY::uint &_i,double &_net);
		void KARAR_RBF(const double* _In,       int &_oEMIR,const MY::uint &_i,double &_net);
		void KARAR_LVQ(const double* _In, const int &_oEMIR,const MY::uint &_i,double &_net,MY::uint &_info);
		void UYGUNLUK_1	( const int* _In, double &_result,bool _VERBOSE=false);
//		void UYGUNLUK_2	( const int* _In, double &_result);
//		void UYGUNLUK_3	( const int* _In, double &_result);
		int EMIR_YAZ(const double &_net);
		int EXP_KNOW(const int &_nEMIR,const int &_oEMIR,const MY::uint &_i);


	public:
		MY::uint     GET_DIM();
		void	SET_LIM (const MY::uint _ILK,const MY::uint _SON);
		void	GET_SNR (double* aSNR, double* uSNR);

		void	UYGUNLUK			( const double* _In, double &_result,bool _VERBOSE=false);
		void	UYGUNLUK_FAST_LVQ	( const double* _In, double &_result);

		void	YAZ_EMIR	(const std::string NAME, const double* In);
		void	YAZ_TAM_EMIR(const std::string NAME);
		void	YAZ_OPT		(std::string NAME, double* In);
		void	INIT_RBF	(MY::uint id);
static void	OKU_DATA(std::string name,std::vector<std::vector<double>>&rawDATA);
static void GET_DATA(std::vector<std::vector<double>>&_DATA,
					const std::vector<std::vector<double>>&_rawDATA);

		virtual ~KON();
		KON();
	protected:
	};
	/*******************************************************************************/
#endif
/***********************************************************************************/
