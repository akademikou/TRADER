#include <iostream>
#include <fstream>
#include <vector>

#include "TANIMLAR.h"
#include "libFUZZY.h"
/*******************************************************************************/
FUZZY::~FUZZY() {delete []LINE;}
/*******************************************************************************/
FUZZY::FUZZY() {}
/*******************************************************************************/
inline double FUZZY::MF_TRI(double x, double a, double b, double c) {
	return MAX(MIN((x - a) / (b - a), (c - x) / (c - b)), 0);
}
/*********************************************************************************/
inline double FUZZY::MF_TRAP(double x, double a, double b, double c, double d) {
	double temp = MIN((x - a) / (b - a), (d - x) / (d - c));
	return MAX(MIN(temp, 1), 0);
}
/*********************************************************************************/
inline double FUZZY::MF_GAUSS(double x, double c, double s) {
	return exp(-(x - c) * (x - c) / (2 * s * s));
}
/*********************************************************************************/
inline double FUZZY::MF_BELL(double x, double a, double b, double c) {
	double t1 = float((x - c) / a);
	double temp = pow(t1, double(2 * b));
	return 1 / (1 + abs(temp));
}
/*********************************************************************************/
inline double FUZZY::MF_SIG(double x, double a, double c) {
	return 1 / (1 + exp((-a) * (x - c)));
}
/*********************************************************************************/
void FUZZY::BULANIKLASTIR(uniFUZZY &_UNI,const  double &_X) {
	for(int i=0;i<_UNI.nMF;i++){
		MF tmp=_UNI.mf[i];
		double F=0;
		switch(tmp.tip){
			case TRI:	F = MF_TRI  (_X, tmp.a, tmp.b, tmp.c);			break;
			case TRAP:	F = MF_TRAP (_X, tmp.a, tmp.b, tmp.c, tmp.d);	break;
			case GAUSS:	F = MF_GAUSS(_X, tmp.c, tmp.s);					break;
			case BELL:	F = MF_BELL (_X, tmp.a, tmp.b, tmp.c);			break;
			case SIG:	F = MF_SIG  (_X, tmp.a, tmp.c);					break;
			default:	std::cout<<i<<" "<<tmp.tip<<" INVALID MEMBERSHIP FUNTION...\n"; system("pause");
		}
		_UNI.mf[i].fuzz=F;
	}
}
/*******************************************************************************/
void FUZZY::CIKARTIM(	const std::vector<::uniFUZZY> UNI_in,
						const std::vector<std::vector<int>> &_RULES,
						std::vector<double> &_OUTS){
	int id_IN,id_MF,id_OP;
	double X1,X2;
	_OUTS.clear();
	for(int i=0;i<(int)_RULES.size();i++){
		id_IN=_RULES[i][0];
		id_MF=_RULES[i][1];
		X1 = UNI_in[id_IN].mf[id_MF].fuzz;
		for(int j=2;j<_RULES[i].size();j+=3){
			id_OP=_RULES[i][j];
			if(id_OP==AND || id_OP==OR){
				id_IN=_RULES[i][j+1];
				id_MF=_RULES[i][j+2];
				X2 = UNI_in[id_IN].mf[id_MF].fuzz;
				if (id_OP == AND) {X1 = T_NORM(X1, X2);}
				if (id_OP ==  OR) {X1 = S_NORM(X1, X2);}
			}else{break;}
		}
		_OUTS.push_back(X1);
	}
}
/*********************************************************************************/
void FUZZY::IMP_AGG(uniFUZZY &_UNI,
					const std::vector<std::vector<int>>RULES,
					const std::vector<double> &_OUTS,
					double *_LINE){

	int nKURAL=(int)_OUTS.size();
	std::vector<double> tmp;

	double x = _UNI.alt;
	double del=(_UNI.ust-_UNI.alt)/(double)BOY;

	for (int i = 0; i < BOY; i++) {
		BULANIKLASTIR(_UNI,x);
		tmp.clear();
		for (int j = 0; j < nKURAL; j++) {
			int id_MF=RULES[j].back();
			double Mout = _UNI.mf[id_MF].fuzz;
			tmp.push_back(Implicate	(_OUTS[j], Mout));
		}
		x += del;
		_LINE[i] = 0;
//		for (int j = 0; j < nKURAL; j++) { _LINE[i] = _LINE[i] + tmp[j]; }
//		for (int j = 0; j < nKURAL; j++) { _LINE[i] = MAX(_LINE[i], tmp[j]); }
		for (int j = 0; j < nKURAL; j++) { _LINE[i] = _LINE[i] + tmp[j] - _LINE[i] * tmp[j]; }// prob or
	//		std::cout<<_LINE[i]<<"\n";
	}
}
/*********************************************************************************/
double FUZZY::DURULASTIRMA(const uniFUZZY &_UNI, double *_LINE) {
	double T1 = 0;
	double T2 = 0.00001;
	double x = _UNI.alt;
	double del=(_UNI.ust-_UNI.alt)/(double)BOY;
	for (int i = 0; i < BOY; i++) {
		T1 = T1 + _LINE[i] * x;
		T2 = T2 + _LINE[i];
		x += del;

	}
	return T1 / T2;
}
/*********************************************************************************/
void FUZZY::RUN(const std::vector<double> &_gir,std::vector<double> &_SONUC) {
	_SONUC.clear();
	for(int i=0;i<(int)UNI_GIR.size();i++){
		BULANIKLASTIR	(UNI_GIR[i],_gir[i]);
	}
	CIKARTIM(UNI_GIR,RULES,OUTS);
	for(int i=0;i<(int)UNI_CIK.size();i++){
		IMP_AGG	(UNI_CIK[i],RULES,OUTS,LINE);
		_SONUC.push_back(DURULASTIRMA(UNI_CIK[i],LINE));
	}
}
/*********************************************************************************/
int main() {
	system("cls");
	std::vector<::uniFUZZY> UNI_gir;
	std::vector<::uniFUZZY> UNI_cık;
	std::vector<std::vector<int>>RULES;
	/***************************************************************
	std::vector<::MF> mfLIST_i1;
	mfLIST_i1.push_back(MF(TRI,std::vector<double>{-2,-1,0}));
	mfLIST_i1.push_back(MF(TRI,std::vector<double>{-1, 0,1}));
	mfLIST_i1.push_back(MF(TRI,std::vector<double>{ 0, 1,2}));
	std::vector<::MF> mfLIST_i2;
	mfLIST_i2.push_back(MF(TRI,std::vector<double>{-2,-1,0}));
	mfLIST_i2.push_back(MF(TRI,std::vector<double>{-1, 0,1}));
	mfLIST_i2.push_back(MF(TRI,std::vector<double>{ 0, 1,2}));
	std::vector<::MF> mfLIST_o1;
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{-1,0.1}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{ 0,0.1}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{ 1,0.1}));
	
	UNI_gir.push_back(uniFUZZY(-1,1,mfLIST_i1));
	UNI_gir.push_back(uniFUZZY(-1,1,mfLIST_i2));	
	UNI_cık.push_back(uniFUZZY(-1,1,mfLIST_o1));
	
	RULES.push_back({0,0,AND,1,0,OUT,0,0});
	RULES.push_back({0,1,AND,1,0,OUT,0,0});
	RULES.push_back({0,2,AND,1,0,OUT,0,1});
	RULES.push_back({0,0,AND,1,1,OUT,0,0});
	RULES.push_back({0,1,AND,1,1,OUT,0,1});
	RULES.push_back({0,2,AND,1,1,OUT,0,2});
	RULES.push_back({0,0,AND,1,2,OUT,0,1});
	RULES.push_back({0,1,AND,1,2,OUT,0,2});
	RULES.push_back({0,2,AND,1,2,OUT,0,2});
	/***************************************************************
//	fuzzy_rule(diff_MA %is% PS, TRADE %is% BS),
//	fuzzy_rule(diff_MA %is% PM, TRADE %is% BB),
//	fuzzy_rule(diff_MA %is% PL, TRADE %is% SM),
//	fuzzy_rule(diff_MA %is% NS, TRADE %is% SS),
//	fuzzy_rule(diff_MA %is% NM, TRADE %is% SB),
//	fuzzy_rule(diff_MA %is% NL, TRADE %is% BM),
//	fuzzy_rule(diff_MA %is% AZ, TRADE %is% N)

	RULES.push_back({0,4,OUT,0,4});
	RULES.push_back({0,5,OUT,0,6});
	RULES.push_back({0,6,OUT,0,1});
	RULES.push_back({0,2,OUT,0,2});
	RULES.push_back({0,1,OUT,0,0});
	RULES.push_back({0,0,OUT,0,5});
	RULES.push_back({0,3,OUT,0,3});

	std::vector<::MF> mfLIST_i1;
	mfLIST_i1.push_back(MF(GAUSS,std::vector<double>{-3, 0.45}));
	mfLIST_i1.push_back(MF(GAUSS,std::vector<double>{-2, 0.45}));
	mfLIST_i1.push_back(MF(GAUSS,std::vector<double>{-1, 0.45}));
	mfLIST_i1.push_back(MF(GAUSS,std::vector<double>{ 0, 0.45}));
	mfLIST_i1.push_back(MF(GAUSS,std::vector<double>{ 1, 0.45}));
	mfLIST_i1.push_back(MF(GAUSS,std::vector<double>{ 2, 0.45}));
	mfLIST_i1.push_back(MF(GAUSS,std::vector<double>{ 3, 0.45}));
	std::vector<::MF> mfLIST_o1;
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{-3, 0.45}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{-2, 0.45}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{-1, 0.45}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{ 0, 0.45}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{ 1, 0.45}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{ 2, 0.45}));
	mfLIST_o1.push_back(MF(GAUSS,std::vector<double>{ 3, 0.45}));
	
	UNI_gir.push_back(uniFUZZY(-3,3,mfLIST_i1));
	UNI_cık.push_back(uniFUZZY(-3,3,mfLIST_o1));
	/***************************************************************/
	/* input id's */
	const int LOW		=0, NORMAL		=1, HIGH		=2;
	const int Oversold	=0, Neutral		=1, Overbought	=2;
	const int SELL_TRADE=0, HOLD_TRADE	=1, BUY_TRADE	=2;

	const int BB_UPPER		=0;
	const int BB_LOWER		=1;
	const int EMA			=2;
	const int HISTOGRAM		=3;
	const int MACD_LINE		=4;
	const int SIGNAL_LINE	=5;
	const int OBV			=6;
	const int PC_UPPER		=7;
	const int PC_LOWER		=8;
	const int ROC			=9;
	const int SMAC			=10;
	const int SMA			=11;
	const int RSI			=12;
	const int STOCH_K		=13;
	const int STOCH_D		=14;

	std::vector<::MF> mfLIST_i0 ,mfLIST_i1 ,mfLIST_i2 ,mfLIST_i3 ,mfLIST_i4;
	std::vector<::MF> mfLIST_i5 ,mfLIST_i6 ,mfLIST_i7 ,mfLIST_i8 ,mfLIST_i9;
	std::vector<::MF> mfLIST_i10,mfLIST_i11,mfLIST_i12,mfLIST_i13,mfLIST_i14;
	std::vector<::MF> mfLIST_o0;

	/***************************************************************/
	std::vector<double>PAR={5,5,5,5,5,
							5,5,5,5,5,
							5,5,
							25,75,
							25,75,
							25,75};
	double Threshold_BB_U	=PAR[0];
	double Threshold_BB_L	=PAR[1];
	double Threshold_EMA	=PAR[2];
	double Threshold_HIST	=PAR[3];
	double Threshold_MACD	=PAR[4];
	double Threshold_SIGN	=PAR[5];
	double Threshold_OBV	=PAR[6];
	double Threshold_PC_U	=PAR[7];
	double Threshold_PC_L	=PAR[8];
	double Threshold_ROC	=PAR[9];
	double Threshold_SMAC	=PAR[10];
	double Threshold_SMA	=PAR[11];
	double Oversold_LEVEL_RSI		=PAR[12];
	double Overbought_LEVEL_RSI		=PAR[13];
	double Oversold_LEVEL_STOCH_K	=PAR[14];
	double Overbought_LEVEL_STOCH_K	=PAR[15];
	double Oversold_LEVEL_STOCH_D	=PAR[16];
	double Overbought_LEVEL_STOCH_D	=PAR[17];
	std::cout<<"Oversold_LEVEL_STOCH_K "<<Oversold_LEVEL_STOCH_K<<std::endl;
	double Mid_Oversold_LEVEL_RSI		= (50 - ((50-Oversold_LEVEL_RSI  ) / 2.0));
	double Mid_Overbought_LEVEL_RSI		= (50 + ((Overbought_LEVEL_RSI-50) / 2.0));
	double Mid_Oversold_LEVEL_STOCH_K	= (50 - ((50-Oversold_LEVEL_STOCH_K  ) / 2.0));
	double Mid_Overbought_LEVEL_STOCH_K	= (50 + ((Overbought_LEVEL_STOCH_K-50) / 2.0));
  	double Mid_Oversold_LEVEL_STOCH_D	= (50 - ((50-Oversold_LEVEL_STOCH_D  ) / 2.0));
	double Mid_Overbought_LEVEL_STOCH_D	= (50 + ((Overbought_LEVEL_STOCH_D-50) / 2.0));
	/****************************************************************************/
	mfLIST_i0.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_BB_U, 0}));
	mfLIST_i0.push_back(MF(TRI ,std::vector<double>{-Threshold_BB_U/2, 0,Threshold_BB_U/2}));
	mfLIST_i0.push_back(MF(TRAP,std::vector<double>{0,Threshold_BB_U, 25,100}));
	/****************************************************************************/
	mfLIST_i1.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_BB_L, 0}));
	mfLIST_i1.push_back(MF(TRI ,std::vector<double>{-Threshold_BB_L/2, 0,Threshold_BB_L/2}));
	mfLIST_i1.push_back(MF(TRAP,std::vector<double>{0,Threshold_BB_L, 25,100}));
	/****************************************************************************/
	mfLIST_i2.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_EMA, 0}));
	mfLIST_i2.push_back(MF(TRI ,std::vector<double>{-Threshold_EMA/2, 0,Threshold_EMA/2}));
	mfLIST_i2.push_back(MF(TRAP,std::vector<double>{0,Threshold_EMA, 25,100}));
	/****************************************************************************/
	mfLIST_i3.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_HIST, 0}));
	mfLIST_i3.push_back(MF(TRI ,std::vector<double>{-Threshold_HIST/2, 0,Threshold_HIST/2}));
	mfLIST_i3.push_back(MF(TRAP,std::vector<double>{0,Threshold_HIST, 25,100}));
	/****************************************************************************/
	mfLIST_i4.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_MACD, 0}));
	mfLIST_i4.push_back(MF(TRI ,std::vector<double>{-Threshold_MACD/2, 0,Threshold_MACD/2}));
	mfLIST_i4.push_back(MF(TRAP,std::vector<double>{0,Threshold_MACD, 25,100}));
	/****************************************************************************/
	mfLIST_i5.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_SIGN, 0}));
	mfLIST_i5.push_back(MF(TRI ,std::vector<double>{-Threshold_SIGN/2, 0,Threshold_SIGN/2}));
	mfLIST_i5.push_back(MF(TRAP,std::vector<double>{0,Threshold_SIGN, 25,100}));
	/****************************************************************************/
	mfLIST_i6.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_OBV, 0}));
	mfLIST_i6.push_back(MF(TRI ,std::vector<double>{-Threshold_OBV/2, 0,Threshold_OBV/2}));
	mfLIST_i6.push_back(MF(TRAP,std::vector<double>{0,Threshold_OBV, 25,100}));
	/****************************************************************************/
	mfLIST_i7.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_PC_U, 0}));
	mfLIST_i7.push_back(MF(TRI ,std::vector<double>{-Threshold_PC_U/2, 0,Threshold_PC_U/2}));
	mfLIST_i7.push_back(MF(TRAP,std::vector<double>{0,Threshold_PC_U, 25,100}));
	/****************************************************************************/
	mfLIST_i8.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_PC_L, 0}));
	mfLIST_i8.push_back(MF(TRI ,std::vector<double>{-Threshold_PC_L/2, 0,Threshold_PC_L/2}));
	mfLIST_i8.push_back(MF(TRAP,std::vector<double>{0,Threshold_PC_L, 25,100}));
	/****************************************************************************/
	mfLIST_i9.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_ROC, 0}));
	mfLIST_i9.push_back(MF(TRI ,std::vector<double>{-Threshold_ROC/2, 0,Threshold_ROC/2}));
	mfLIST_i9.push_back(MF(TRAP,std::vector<double>{0,Threshold_ROC, 25,100}));
	/****************************************************************************/
	mfLIST_i10.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_SMAC, 0}));
	mfLIST_i10.push_back(MF(TRI ,std::vector<double>{-Threshold_SMAC/2, 0,Threshold_SMAC/2}));
	mfLIST_i10.push_back(MF(TRAP,std::vector<double>{0,Threshold_SMAC, 25,100}));
	/****************************************************************************/
	mfLIST_i11.push_back(MF(TRAP,std::vector<double>{-100,-25, -Threshold_SMA, 0}));
	mfLIST_i11.push_back(MF(TRI ,std::vector<double>{-Threshold_SMA/2, 0,Threshold_SMA/2}));
	mfLIST_i11.push_back(MF(TRAP,std::vector<double>{0,Threshold_SMA, 25,100}));
	/****************************************************************************/
	mfLIST_i12.push_back(MF(TRAP,std::vector<double>{-0.1, 0, Oversold_LEVEL_RSI, 50}));
	mfLIST_i12.push_back(MF(TRI ,std::vector<double>{Mid_Oversold_LEVEL_RSI, 50,Mid_Overbought_LEVEL_RSI}));
	mfLIST_i12.push_back(MF(TRAP,std::vector<double>{50, Overbought_LEVEL_RSI, 100,100.1}));
	/****************************************************************************/
	mfLIST_i13.push_back(MF(TRAP,std::vector<double>{-0.1, 0, Oversold_LEVEL_STOCH_K, 50}));
	mfLIST_i13.push_back(MF(TRI ,std::vector<double>{Mid_Oversold_LEVEL_STOCH_K, 50,Mid_Overbought_LEVEL_STOCH_K}));
	mfLIST_i13.push_back(MF(TRAP,std::vector<double>{50, Overbought_LEVEL_STOCH_K, 100,100.1}));
	/****************************************************************************/
	mfLIST_i14.push_back(MF(TRAP,std::vector<double>{-0.1, 0, Oversold_LEVEL_STOCH_D, 50}));
	mfLIST_i14.push_back(MF(TRI ,std::vector<double>{Mid_Oversold_LEVEL_STOCH_D, 50,Mid_Overbought_LEVEL_STOCH_D}));
	mfLIST_i14.push_back(MF(TRAP,std::vector<double>{50, Overbought_LEVEL_STOCH_D, 100,100.1}));
	/****************************************************************************/
	mfLIST_o0.push_back(MF(TRI ,std::vector<double>{-2, -1, 0}));
	mfLIST_o0.push_back(MF(TRI ,std::vector<double>{-1,  0, 1}));
	mfLIST_o0.push_back(MF(TRI ,std::vector<double>{ 0,  1, 2}));
	/****************************************************************************/
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i0 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i1 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i2 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i3 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i4 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i5 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i6 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i7 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i8 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i9 ));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i10));
	UNI_gir.push_back(uniFUZZY(-25, 25,mfLIST_i11));
	UNI_gir.push_back(uniFUZZY(  0,100,mfLIST_i12));
	UNI_gir.push_back(uniFUZZY(  0,100,mfLIST_i13));
	UNI_gir.push_back(uniFUZZY(  0,100,mfLIST_i14));
	UNI_cık.push_back(uniFUZZY( -2,  2,mfLIST_o0 ));

/*	RULES.push_back({BB_UPPER	,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({BB_UPPER	,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({BB_LOWER	,LOW   ,OUT,0,SELL_TRADE});
	RULES.push_back({BB_LOWER	,NORMAL,OUT,0,HOLD_TRADE});    
	RULES.push_back({EMA		,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({EMA		,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({EMA		,LOW   ,OUT,0,SELL_TRADE});    
	RULES.push_back({HISTOGRAM	,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({HISTOGRAM	,LOW   ,OUT,0,SELL_TRADE});
	RULES.push_back({MACD_LINE	,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({MACD_LINE	,LOW   ,OUT,0,SELL_TRADE});
	RULES.push_back({SIGNAL_LINE,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({SIGNAL_LINE,LOW   ,OUT,0,SELL_TRADE});    
	RULES.push_back({OBV		,HIGH  ,OUT,0,BUY_TRADE});
	RULES.push_back({OBV		,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({OBV		,LOW   ,OUT,0,SELL_TRADE});    
	RULES.push_back({PC_UPPER	,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({PC_UPPER	,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({PC_UPPER	,LOW   ,OUT,0,SELL_TRADE});
	RULES.push_back({PC_LOWER	,LOW   ,OUT,0,SELL_TRADE});
	RULES.push_back({PC_LOWER	,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({PC_LOWER	,HIGH  ,OUT,0,BUY_TRADE });    
	RULES.push_back({ROC		,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({ROC		,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({ROC		,LOW   ,OUT,0,SELL_TRADE});    
	RULES.push_back({SMAC		,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({SMAC		,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({SMAC		,LOW   ,OUT,0,SELL_TRADE});    
	RULES.push_back({SMA		,HIGH  ,OUT,0,BUY_TRADE });
	RULES.push_back({SMA		,NORMAL,OUT,0,HOLD_TRADE});
	RULES.push_back({SMA		,LOW   ,OUT,0,SELL_TRADE});
	RULES.push_back({RSI		,Overbought,OUT,0,SELL_TRADE});
	RULES.push_back({RSI		,Neutral   ,OUT,0,HOLD_TRADE});
	RULES.push_back({RSI		,Oversold  ,OUT,0,BUY_TRADE });*/
	RULES.push_back({STOCH_K	,Overbought,OUT,0,SELL_TRADE});
	RULES.push_back({STOCH_K	,Neutral   ,OUT,0,HOLD_TRADE});
	RULES.push_back({STOCH_K	,Oversold  ,OUT,0,BUY_TRADE });
	RULES.push_back({STOCH_D	,Overbought,OUT,0,SELL_TRADE});
	RULES.push_back({STOCH_D	,Neutral   ,OUT,0,HOLD_TRADE});
	RULES.push_back({STOCH_D	,Oversold  ,OUT,0,BUY_TRADE });
	/****************************************************************************/
	FUZZY myFUZZY(UNI_gir,UNI_cık,RULES);
	/********************************************/
	std::fstream DOSYA;
	DOSYA.open("FUZZY_CPP2.txt", std::ios::out);
	if (!DOSYA.is_open()) {
		std::cout << "DOSYA OLUSTURULAMADI.  KULLANICI IZINLERINI KONTROL ET" << std::endl;
		exit(1);
	}
	std::vector<double>IN(18,0);
	std::vector<double>OUT={0};
	std::cout<<UNI_gir[0].alt<<std::endl;
	std::cout<<UNI_gir[0].ust<<std::endl;
	const int id_I0=13;
	const int id_I1=14;
	for (int i = 0; i < BOY_1D; i++) {
		for (int j = 0; j < BOY_1D; j++) {
			IN[id_I0] = UNI_gir[id_I0].alt+i*(UNI_gir[id_I0].ust-UNI_gir[id_I0].alt)/(double)BOY_1D;
			IN[id_I1] = UNI_gir[id_I1].alt+j*(UNI_gir[id_I1].ust-UNI_gir[id_I1].alt)/(double)BOY_1D;
			myFUZZY.RUN(IN,OUT);
			std::cout<< i<<"\t"<<j<<"\t"<<IN[id_I0]<<"\t"<<IN[id_I1]<<"\t"<<OUT[0]<<std::endl;
			    DOSYA<< i<<"\t"<<j<<"\t"<<IN[id_I0]<<"\t"<<IN[id_I1]<<"\t"<<OUT[0]<<std::endl;
		}
	}
	DOSYA.close();
	return 0;
}
/*******************************************************************************/