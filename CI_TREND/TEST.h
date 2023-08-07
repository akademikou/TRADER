#pragma once
/**************************************************************************************/
void R_TEST(const std::vector<std::vector<double>> &DATA,const std::vector<PRICE> &FYT,const double P1,const double P2,
	PRICE& _FYT,double& _tCURR, double& _tUSDT){
	int LAST=0;
	int ilk = 100+0*(int)DATA.size()/2;
	int son = (int)DATA.size()-FWD-1;
	std::vector<double> order_BUY;
	std::vector<double> order_SELL;
	const double commision = 0.000;
	double TOTAL_FUND = 10000;
	double tCURR = TOTAL_FUND *0.5 / FYT[ilk].CLOSE;
	double tUSDT = TOTAL_FUND *0.5;
	const double UNIT = (500/ FYT[ilk].CLOSE) / (1.0 - commision); // 50 dolarlık
	bool oS = false;
	bool oA = false;
	std::vector<ORDER> order_LIST;
	order_LIST.clear();

	int Y_trade=0;
	for (int i = ilk; i <=son; i++) {
		eval_ORDER(FYT[i], commision, tUSDT, tCURR, order_LIST);
		double RSI = DATA[i][0];
		double ADX = DATA[i][1];
		double DIp = DATA[i][2];
		double DIn = DATA[i][3];

//		std::cout<<RSI<<"\t"<<ADX<<"\t"<<DIp<<"\t"<<DIn<<"\n";
//		double ARN = DATA[i][1];
//		double STOC_F = DATA[i][2];
//		double STOC_D = DATA[i][3];
//		oSAT = RSI > 70 || (ARN >  70 && STOC_F > 80 && STOC_D > 80);
//		oAL  = RSI < 30 || (ARN < -70 && STOC_F < 20 && STOC_D < 20);

//		oA = (RSI < P1) ;
//		oS = (RSI > P2) ;

		oA = (ADX > P1) && (DIp-DIn)> P2;
		oS = (ADX > P1) && (DIp-DIn)<-P2;

		if(!oA && Y_trade== 1){oS=true;}
		if(!oS && Y_trade==-1){oA=true;}
		if(oA){Y_trade++;if(Y_trade> 1){oA=false;Y_trade--;}}
		if(oS){Y_trade--;if(Y_trade<-1){oS=false;Y_trade++;}}


		if (oS){order_LIST.push_back(ORDER(FYT[i].CLOSE, UNIT, eMARKET, eSAT));}	// DUSECEK SAT
		if (oA){order_LIST.push_back(ORDER(FYT[i].CLOSE, UNIT, eMARKET, eAL ));}	// YUKSELECEK AL

	//	if (oS){order_LIST.push_back(ORDER(FYT[i].CLOSE*1.003, UNIT, eLIMIT, eSAT));}// DUSECEK SAT
	//	if (oA){order_LIST.push_back(ORDER(FYT[i].CLOSE*0.997, UNIT, eLIMIT, eAL ));}// YUKSELECEK AL

//		if (i%50==0 || i==son){info_ASSET(FYT[i], tCURR, tUSDT);}
		oS=oA=false;
	}
//	std::cout << "__________________________________________________________________"<<UP<<"\t"<<DOWN << std::endl;
//	info_ASSET(FYT[son], tCURR, tUSDT);
	_FYT=FYT[son];
	_tCURR=tCURR;
	_tUSDT=tUSDT;
}
/**************************************************************************************/