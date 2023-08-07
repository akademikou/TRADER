#pragma once
#ifndef CLASS_FUZZY_H
#define CLASS_FUZZY_H

//#define U_DOMBI
//#define U_DUBOIS
//#define U_YAGER
//#define U_DRASTIC
//#define U_EINSTEIN
#define U_ALGEBRIC
//#define U_MAX

//#define I_DOMBI
//#define I_DUBOIS
//#define I_YAGER
//#define I_DRASTIC
//#define I_EINSTEIN
#define I_ALGEBRIC
//#define I_MAX

//#define Imp_DIENES
//#define Imp_LUKA
//#define Imp_ZADEH
//#define Imp_GODEL
#define Imp_MAMDANI_1
//#define Imp_MAMDANI_2
/**********************************************************************************************/
class MF {
public: 
	int tip=0;
 	double a, b, c, d, s;
	double fuzz;
public:
	MF(){tip=0;a=b=c=d=s=fuzz=0;}
	MF(int _tip,std::vector<double>par){
		a=b=c=d=s=fuzz=0;
		if(_tip<TRI || _tip>SIG){std::cout<<"invalid MF type! returning...\n"; return;}
		tip=_tip;
		int nPar=(int)par.size();
		switch(tip){
		case TRI:
			if(nPar!=3){std::cout<<"invalid parameter! returning...\n"; return;}
			a=par[0];
			b=par[1];
			c=par[2];
			break;
		case TRAP:
			if(nPar!=4){std::cout<<"invalid parameter! returning...\n"; return;}
			a=par[0];
			b=par[1];
			c=par[2];
			d=par[3];
			break;
		case GAUSS:
			if(nPar!=2){std::cout<<"invalid parameter! returning...\n"; return;}
			c=par[0];
			s=par[1];
			break;
		case BELL:
			if(nPar!=3){std::cout<<"invalid parameter! returning...\n"; return;}
			a=par[0];
			b=par[1];
			c=par[2];
			break;
		case SIG:
			if(nPar!=2){std::cout<<"invalid parameter! returning...\n"; return;}
			a=par[0];
			c=par[1];
			break;
		}
	}
};
/**********************************************************************************************/
class uniFUZZY {
public:
	int nMF=0;
	std::vector<MF>mf;
	double alt,ust;
protected:
	
public:
	uniFUZZY(const double _alt,const double _ust,const std::vector<::MF>&_MF){
		mf.clear();
		alt=_alt;
		ust=_ust;
		nMF=(int)_MF.size();
		for(int i=0;i<nMF;i++){
			mf.push_back(_MF[i]);
			std::cout<<i<<" "<<mf[i].tip<<"\n";
		}
	}
};
/**********************************************************************************************/
class FUZZY {
protected:
	const int BOY=100;

	double *LINE;
	std::vector<uniFUZZY> UNI_GIR;
	std::vector<uniFUZZY> UNI_CIK;
	std::vector<std::vector<int>>RULES;
	std::vector<double>OUTS;

protected:

public:
	void RUN(const std::vector<double> &_gir,std::vector<double> &_SONUC);
	virtual ~FUZZY();
	FUZZY();
	FUZZY(const std::vector<::uniFUZZY> &_GIR,
		  const std::vector<uniFUZZY> &_CIK,
		  const std::vector<std::vector<int>>_RULES){
		for(int i=0;i<(int)_GIR.size();i++){UNI_GIR.push_back(_GIR[i]);}
		for(int i=0;i<(int)_CIK.size();i++){UNI_CIK.push_back(_CIK[i]);}
		for(int i=0;i<(int)_RULES.size();i++){
			RULES.push_back(_RULES[i]);
		}
		LINE=new double[BOY];
	}

protected:
	void BULANIKLASTIR(uniFUZZY &_UNI,const  double &_X);
	void CIKARTIM(	const std::vector<::uniFUZZY> _INPUT,
					const std::vector<std::vector<int>> &_RULES,
					std::vector<double> &_OUTS);
	void IMP_AGG(	uniFUZZY &_UNI,
					const std::vector<std::vector<int>>RULES,
					const std::vector<double> &_OUTS,
					double *_LINE);
	double DURULASTIRMA(const uniFUZZY &_UNI, double *_LINE);
	/*********************************************************************************/ // ÜYELİK FONKSİYONLARI
	inline double MF_TRI(double x, double a, double b, double c);
	inline double MF_TRAP(double x, double a, double b, double c, double d);
	inline double MF_GAUSS(double x, double c, double s);
	inline double MF_BELL(double x, double a, double b, double c);
	inline double MF_SIG(double x, double a, double c);

	inline double S_NORM(double a, double b);			// VEYA BAĞLACI
	inline double T_NORM(double a, double b);			// VE BAĞLACI
	inline double Implicate(double Min, double Mout);	// IMPLICATION YÖNTEMLERİ
};
#endif
/**********************************************************************************************/
inline double FUZZY::S_NORM(double a, double b){
#if defined U_DOMBI
	double Lambda=3;
	double temp1 = pow(1 / a - 1, -Lambda);
	double temp2 = pow(1 / b - 1, -Lambda);
	double temp3 = pow(temp1 + temp2, -1 / Lambda);
	return 1 / (1 + temp3);
#elif defined U_DUBOIS
	double alfa=0;
	double temp1 = MIN(1 - alfa, MIN(a, b));
	double temp2 = MAX(alfa, MAX(1 - a, 1 - b));
	return (a + b - a * b - temp1) / temp2;
#elif defined U_YAGER
	double w=3;
	double temp1 = pow(a, w);
	double temp2 = pow(b, w);
	double temp3 = pow(temp1 + temp2, 1 / w);
	return MIN(1, temp3);
#elif defined U_DRASTIC
	if (a == 0) { return b; }
	if (b == 0) { return a; }
	return 1;
#elif defined U_EINSTEIN
	return (a + b) / (1 + a * b);
#elif defined U_ALGEBRIC
	return a + b - a * b; 
#elif defined U_MAX
	return MAX(a, b);
#endif
}
/*********************************************************************************/
inline double FUZZY::T_NORM(double a, double b){
#if defined I_DOMBI
	double Lambda=3;
	a = MAX(0.0001, a);
	b = MAX(0.0001, b);
	double temp1 = pow((1 / a) - 1, Lambda);
	double temp2 = pow((1 / b) - 1, Lambda);
	double temp3 = pow(temp1 + temp2, 1 / Lambda);
	return 1 / (1 + temp3);
#elif defined I_DUBOIS
	double alfa=0;
	double temp1 = MAX(alfa, MAX(a, b));
	temp1 = MAX(0.0001, temp1);
	return (a * b) / temp1;
#elif defined I_YAGER
	double w=3;
	double temp1 = pow((1 - a), w);
	double temp2 = pow((1 - b), w);
	double temp3 = pow(temp1 + temp2, 1 / w);
	return 1 - MIN(1, temp3);
#elif defined I_DRASTIC
	if (a == 1) { return b; };
	if (b == 1) { return a; };
	return 0;
#elif defined I_EINSTEIN
	return (a * b) / (2 - (a + b - a * b)); 
#elif defined I_ALGEBRIC
	return a * b; 
#elif defined I_MAX
	return MIN(a, b);
#endif
}
/*********************************************************************************/
inline double FUZZY::Implicate(double Min, double Mout){
#if defined Imp_DIENES
	return MAX(1 - Min, Mout);
#elif defined Imp_LUKA
	return MIN(1, 1 - Min + Mout);
#elif defined Imp_ZADEH
	return MAX(MIN(Min, Mout), 1 - Min);
#elif defined Imp_GODEL
	if (Min < Mout) { return 1; } else { return Mout; }
#elif defined Imp_MAMDANI_1
	return MIN(Min, Mout);
#elif defined Imp_MAMDANI_2
	return Min * Mout;
#endif
}
/*******************************************************************************/