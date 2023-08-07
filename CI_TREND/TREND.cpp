#pragma warning (disable:4251)
#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#undef BINARY 
//#define BINARY 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm> // for sort()
#include <omp.h>
#include <bitset>
#include <random>

#include "CLASS_INDICATOR.h"
const double Span = 0.5;
const int nPSS = 1;
const int nCIK = 3;
const int FWD = 20;
const int BWD = 0;
//const int nGIR = 22*BWD;
const int nINP = 1*BWD;
#include "MATRIX.h"

#include "CLASS_LVQ.h"
#include "AUXILARY.h"
#include "TRAIN.h"
#include "TRADE.h"
#include "TEST.h"
/*******************************************************************************/
void main() {
	system("cls");
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout << std::setprecision(6);
	/**************************************************************************************/
//	const unsigned int numData = 60*24*60;// 60 gün 86400
	const unsigned int numData = 60*24*5;// 1 gün 86400
	const std::string interval = "1m";
	INDICATOR* CRYPT_INDICATOR = new INDICATOR("BTCUSDT", interval.c_str(), numData);	
	/**************************************************************************************/
//	CRYPT_INDICATOR->GET_DATA();
//	CRYPT_INDICATOR->CALC();
//	CRYPT_INDICATOR->YAZ_DATA();
	CRYPT_INDICATOR->OKU_DATA();
	
	int BOY=CRYPT_INDICATOR->numData;

	std::vector<double> vOUT(BOY, 0);
	std::vector<std::vector<double>> vIN;
	std::vector<PRICE>FYT;

	CALC_IN (*CRYPT_INDICATOR,vIN ,BWD);
	CALC_OUT(*CRYPT_INDICATOR,vOUT,FWD, Span);
	CALC_FIYAT(*CRYPT_INDICATOR,FYT);


	/**************************************************************************************/	
	std::vector<std::vector<double>> sorted_DATA;
	std::vector<std::vector<double>> DATA;	
	PROCESS_TRAIN_SET(nCIK,Span,vIN,vOUT,DATA,sorted_DATA);

	std::vector<std::vector<double>> T_DATA;
	for(int i=CRYPT_INDICATOR->valData;i<(int)DATA.size()-FWD;i++){T_DATA.push_back(DATA[i]);}
//	for(int i=CRYPT_INDICATOR->valData;i<(int)DATA.size();i++){T_DATA.push_back(sorted_DATA[i]);}
//	sorted_DATA.clear();
//	for(int i=0;i<(int)T_DATA.size();i++){for(int j=0;j<(int)T_DATA[i].size();j++){std::cout<<T_DATA[i][j]<<"\t";} std::cout<<"\n";}
	/**************************************************************************************/
	
	const int nH = nCIK * nPSS;
	const double OK = 5E-3;
	const int MID = int(nCIK / 2);
	int nGIR=(int)vIN[0].size();
	double KonErr = 0;	

	double** W = Init_Matrix_2D<double>(nH, nGIR);
	double* G_YSA = Init_Matrix_1D<double>(nGIR);
	LVQ* YSA_LVQ = new LVQ(nGIR, nCIK, nPSS, OK);
	INIT_W(W,T_DATA, nGIR, nH, nPSS);

	std::cout<<"____________________________________________________________________________________\n";
	std::cout<<"W[i][j]\n";
	for (int i = 0; i < nH; i++) {
		for (int j = 0; j < nGIR; j++) {std::cout<<W[i][j]<<"\t"; }	std::cout<<"\n"; 
	}
	std::cout<<"____________________________________________________________________________________\n";
	YSA_LVQ->set_W(W);
	TRAIN(W, T_DATA, YSA_LVQ, nGIR, MID,  KonErr);
	YSA_LVQ->get_W(W);
	std::cout<<"____________________________________________________________________________________\n";
	std::cout<<"W[i][j]\n";
	for (int i = 0; i < nH; i++) {
		for (int j = 0; j < nGIR; j++) {std::cout<<W[i][j]<<"\t"; }	std::cout<<"\n"; 
	}
	std::cout<<"____________________________________________________________________________________\n";
	
	std::cout<<"HUCRELERIN KAZANMA SAYISI\n";
//	for (int i = 0; i < nH; i++) {std::cout<<YSA_LVQ->sH[i]<<"\n";}
	/**************************************************************************************/
	PRICE rFYT;
	double rCURR,rUSDT;
	std::fstream rDOSYA;
	rDOSYA.open("RES.txt",std::ios::out);

	for(double P1=0,i=1;P1<70;P1+=1,i++){
		std::cout<<P1<<std::endl;
		for(double P2=0,j=1;P2<70;P2+=1,j++){
			R_TEST(DATA,FYT,P1,P2,rFYT,rCURR, rUSDT);
			rDOSYA	<<i<<"\t"
					<<j<<"\t"
					<<P1<<"\t"
					<<P2<<"\t"					
					<<rUSDT + rCURR * rFYT.CLOSE<<"\t"
					<<rCURR + rUSDT / rFYT.CLOSE<<"\t"
					<<rUSDT<<"\t"
					<<rCURR<<"\n";
		}
	}	
	rDOSYA.close();
//	return;
	/**************************************************************************************/
	int LAST=1;
	int ilk = (int)DATA.size()/2;
	int son = (int)DATA.size()-FWD-1;
	std::vector<double> order_BUY;
	std::vector<double> order_SELL;
	const double commision = 0.000;
	int estimate;
	double TOTAL_FUND = 10000;
	double tCURR = TOTAL_FUND *0.5 / FYT[ilk].CLOSE;
	double tUSDT = TOTAL_FUND *0.5;
	const double UNIT = (50/ FYT[ilk].CLOSE) / (1.0 - commision); // 50 dolarlık
	double err=0;
	double G_err = 0;
	std::cout << "__________________________________________________________________\n";
	info_ASSET(FYT[ilk], tCURR, tUSDT);
	info_ASSET(FYT[son], tCURR, tUSDT);
	std::cout << "real test working...\n";
	std::cout << "__________________________________________________________________\n";
	std::fstream DOSYA;
	DOSYA.open("emir.txt",std::ios::out);

	KonErr *= 1.5;
	bool oS = false;
	bool oA = false;
	std::vector<ORDER> order_LIST;
	order_LIST.clear();
	int Y_trade=0;
	for (int i = ilk; i <=son; i++) {
		eval_ORDER(FYT[i], commision, tUSDT, tCURR, order_LIST);
		estimate = estimate_MARKET(DATA, G_YSA, YSA_LVQ, nGIR, i, err);

		double RSI = DATA[i][0];
		double ADX = DATA[i][1];
		double DIp = DATA[i][2];
		double DIn = DATA[i][3];

//		oS = RSI > 70 || (ARN >  70 && STOC_F > 80 && STOC_D > 80);
//		oA = RSI < 30 || (ARN < -70 && STOC_F < 20 && STOC_D < 20);
//		oS = estimate < (MID ) && SELL && tCURR > UNIT ;
//		oA = estimate > (MID ) && BUY && tUSDT > 50 ;

//		oS = estimate < (MID+1) ;
//		oA = estimate > (MID+1) ;

//		oA = (RSI < 30) ;
//		oS = (RSI > 70) ;


		oA = (ADX > 20) && (DIp-DIn)> 1;
		oS = (ADX > 20) && (DIp-DIn)<-1;

		if(!oA && Y_trade== 1){oS=true;}
		if(!oS && Y_trade==-1){oA=true;}
		if(oA){Y_trade++;if(Y_trade> 1){oA=false;Y_trade--;}}
		if(oS){Y_trade--;if(Y_trade<-1){oS=false;Y_trade++;}}
		
		
		
		if (oS){order_LIST.push_back(ORDER(FYT[i].CLOSE, UNIT, eMARKET, eSAT));}	// DUSECEK SAT
		if (oA){order_LIST.push_back(ORDER(FYT[i].CLOSE, UNIT, eMARKET, eAL ));}	// YUKSELECEK AL

	//	if (oS){order_LIST.push_back(ORDER(FYT[i].CLOSE*1.003, UNIT, eLIMIT, eSAT));}// DUSECEK SAT
	//	if (oA){order_LIST.push_back(ORDER(FYT[i].CLOSE*0.997, UNIT, eLIMIT, eAL ));}// YUKSELECEK AL

		if (i%50==0 || i==son){
			G_err += err;
//			std::cout << KonErr << "\t" <<  G_err / 50 << "\t";
			info_ASSET(FYT[i], tCURR, tUSDT);			
			G_err = 0;
		}
		if( oS){DOSYA<<i+1<<"\t"<<DATA[i][1]<<"\t"<<FYT[i].CLOSE<<"\t"<<Y_trade<<"\tSAT\n";}
		if( oA){DOSYA<<i+1<<"\t"<<DATA[i][1]<<"\t"<<FYT[i].CLOSE<<"\t"<<Y_trade<<"\tAL\n";}
		oS=oA=false;
	}
	DOSYA.close();
	std::cout << "\n__________________________________________________________________" << std::endl;
	std::cout << "real test finished...\n";
	info_ASSET(FYT[son], tCURR, tUSDT);
	tCURR = TOTAL_FUND*0.5 / FYT[ilk].CLOSE;
	tUSDT = TOTAL_FUND *0.5;
	info_ASSET(FYT[son], tCURR, tUSDT);
	std::cout << "__________________________________________________________________" << std::endl;

	double AL1 = 0, SAT1 = 0;
	double AL2 = 0, SAT2 = 0;
	int iA = 0, iS = 0;
	
	for (int i = 0; i < order_LIST.size(); i++) {
	/*	std::string info1 = order_LIST[i].TYPE == eMARKET ? "MARKET" : "LIMIT";
		std::string info2 = order_LIST[i].DIR==eAL? "AL":"SAT";
		std::cout	<< order_LIST[i].done_USD << "\t"
					<< order_LIST[i].value_USD << "\t"
					<< info1 << "\t"
					<< info2 << "\t"
					<< order_LIST[i].DONE << std::endl;
	*/
		if (order_LIST[i].DONE) {
			if (order_LIST[i].DIR > 0) { 
				AL1 += order_LIST[i].done_USD;
				AL2 += order_LIST[i].value_USD;
				iA++; 
			}
			if (order_LIST[i].DIR < 0) {
				SAT1 += order_LIST[i].done_USD;
				SAT2 += order_LIST[i].value_USD;
				iS++;
			}
		}
	}
	std::cout << "AL :" << iA<<"\t"<<AL1/iA << "\t"<<AL2/iA << std::endl;
	std::cout << "SAT:" << iS<<"\t"<<SAT1/iS << "\t"<<SAT2/iS << std::endl;


	system("pause");
	DATA.clear();
	T_DATA.clear();
}
/************************************************************************************/


/************************************************************************************/