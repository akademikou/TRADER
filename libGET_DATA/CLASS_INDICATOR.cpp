#pragma warning (disable:4244)
#pragma warning (disable:4251)
#pragma warning (disable:4267)
#include <iostream>
#include <fstream>
#include <vector>
#include "ta_libc.h"

#define API_KEY 		"HUuXiG6wOd6tcqeUhOexkres6R5ASKzG2F6uFnbbQtLnvrauoiYhpsMWQb65A5Q0"
#define SECRET_KEY		"mqUTY6qGNAhfG7wlL6kz4mUXGNCC77qQS0aAvuGEOEh1ECwubbszRyE8YqVLn5mV"
std::string api_key = API_KEY;
std::string secret_key = SECRET_KEY;
#include "CLASS_INDICATOR.h"
/**************************************************************************************************************/
INDICATOR::~INDICATOR() {}
/**************************************************************************************************************/
INDICATOR::INDICATOR(){DEALLOC_MEM();}
/**************************************************************************************************************/
INDICATOR::INDICATOR(std::string _name, std::string _interval, long long int _numData){
	name = _name;
	interval = _interval;
	numData = _numData;
	valData = 0;
	MY_CONNECTION.init(api_key, secret_key);
	MY_CONNECTION.get_serverTime(MY_result);
}
/**************************************************************************************************************/
void INDICATOR::GET_DATA() {
	const int maxLINE=500;
	time_t sTime;
	time_t tTime;
	time_t eTime;
	time(&eTime);  /* get current time; same as: timer = time(NULL)  */
	eTime*=1000;
	
	if(interval=="1m"){
		sTime=eTime-numData*60*1000;
		tTime=sTime+maxLINE*60*1000;
	}
	else if(interval=="5m"){
		sTime=eTime-numData*60*1000*5;
		tTime=sTime+maxLINE*60*1000;
	}
	else if(interval=="15m"){
		sTime=eTime-numData*60*1000*15;
		tTime=sTime+maxLINE*60*1000;
	}
	time_t aTime=0;
	int ind=0;
	ALLOC_MEM(numData);
	do{
		MY_CONNECTION.get_klines(name.c_str(), interval.c_str(), maxLINE, sTime, tTime, MY_result);
		
		for (unsigned int j=0; j < MY_result.size(); j++,ind++) {			
			price_open	[ind] = std::stod(MY_result[j][1].asString());
			price_high	[ind] = std::stod(MY_result[j][2].asString());
			price_low	[ind] = std::stod(MY_result[j][3].asString());
			price_close	[ind] = std::stod(MY_result[j][4].asString());
			price_volume[ind] = std::stod(MY_result[j][5].asString());
			aTime=time_t(std::stod(MY_result[j][0].asString()));
			ZAMAN[ind]=aTime;
			time_t tmpTIME=time_t(aTime*0.001);
			tm *gmtm = gmtime(&tmpTIME);
//			tm *gmtm = localtime(&tmpTIME);
			LoutDate[ind] =std::to_string(1900 + gmtm->tm_year)+' ';
			LoutDate[ind]+=std::to_string( 1 + gmtm->tm_mon)+' ';
			LoutDate[ind]+=std::to_string( gmtm->tm_mday)+' ';
			LoutDate[ind]+=std::to_string( 3+gmtm->tm_hour)+':';
			LoutDate[ind]+=std::to_string(gmtm->tm_min)+ ':';
			LoutDate[ind]+=std::to_string(gmtm->tm_sec);

			std::cout<<j<<"\t"<<ind<<"\t"<<numData<<"\t"<<aTime<<"\t"<<LoutDate[ind]<<"\n";
			if(ind>=numData-1){goto loop1;}
		}
		sTime=aTime+60*1000;
		tTime=sTime+maxLINE*60*1000;
		if(interval=="5m"){
			tTime=sTime+maxLINE*60*1000*5;
		}
		if(interval=="15m"){
			tTime=sTime+maxLINE*60*1000*15;
		}

	}while(sTime<eTime );
loop1:
	numData=ind;
//	for (unsigned int j=0; j < numData; j++,ind++) {price_close[ind]=(price_close[ind]+price_open[ind])*0.5;}
	getStart(valData);
}
/**************************************************************************************************************/
void INDICATOR::YAZ_DATA(){
	std::string NAME;	
	std::fstream DOSYA;

	NAME=name+"_KLINES.txt";
	DOSYA.open(NAME.c_str(),std::ios::out);
	for (int j = 0; j <numData ; j++) {
		DOSYA<<ZAMAN[j]<<"\t";
		DOSYA<<price_open[j]<<"\t";
		DOSYA<<price_high[j]<<"\t";
		DOSYA<<price_low[j]<<"\t";
		DOSYA<<price_close[j]<<"\t";
		DOSYA<<price_volume[j]<<"\t";
		DOSYA<<LoutDate[j]<<"\n";
	}
	DOSYA.close();

	NAME=name+"_CALC.txt";
	DOSYA.open(NAME.c_str(),std::ios::out);
	for(int i=0;i<numData;i++){
		DOSYA<<LoutSMA_25[i]<<"\t"
			<<LoutSMA_99[i]<<"\t"
			<<LoutEMA[i]<<"\t"
			<<LoutSTD[i]<<"\t"

			<<LoutUpperBand[i]<<"\t"
			<<LoutMiddleBand[i]<<"\t"
			<<LoutLowerBand[i]<<"\t"

			<<LoutRSI[i]<<"\t"
			<<LoutARN[i]<<"\t"
			<<LoutARN_up[i]<<"\t"
			<<LoutARN_down[i]<<"\t"

			<<LoutMACD[i]<<"\t"
			<<LoutMACDSignal[i]<<"\t"
			<<LoutMACDHist[i]<<"\t"
			<<LoutFastK[i]<<"\t"
			<<LoutFastD[i]<<"\t"
			<<LoutADX[i]<<"\t"
			<<LoutDIp[i]<<"\t"
			<<LoutDIn[i]<<"\n";
	}
	DOSYA.close();
}
/**************************************************************************************************************/
void INDICATOR::OKU_DATA() {
	std::string line;
	std::vector<double> vTmp(19);
	std::vector<std::vector<double>>rawDATA1;
	std::vector<std::vector<double>>rawDATA2;
	rawDATA1.clear();
	rawDATA2.clear();
	std::string NAME;	
	std::fstream DOSYA;
	/***********************************************************/
	NAME=name+"_KLINES.txt";
	DOSYA.open(NAME.c_str(), std::ios::in);
	if(!DOSYA.is_open()){std::cout<<NAME.c_str()<<" not opened\n";}
	while (std::getline(DOSYA, line)){
		std::istringstream iss(line);
		if (!(iss >> vTmp[0] >> vTmp[1] >> vTmp[2]>> vTmp[3] >> vTmp[4] >> vTmp[5])) { break; }
		rawDATA1.push_back(vTmp);
	}
	DOSYA.close();
	/***********************************************************/
	NAME=name+"_CALC.txt";
	DOSYA.open(NAME.c_str(), std::ios::in);
	if(!DOSYA.is_open()){std::cout<<NAME.c_str()<<" not opened\n";}
	while (std::getline(DOSYA, line)){
		std::istringstream iss(line);
//		std::cout<<iss.str()<<std::endl;
		if (!(iss 
			>> vTmp[ 0] >> vTmp[ 1] >> vTmp[ 2] >> vTmp[ 3] >> vTmp[ 4]
			>> vTmp[ 5] >> vTmp[ 6] >> vTmp[ 7] >> vTmp[ 8] >> vTmp[ 9]
			>> vTmp[10] >> vTmp[11] >> vTmp[12] >> vTmp[13] >> vTmp[14]
			>> vTmp[15] >> vTmp[16] >> vTmp[17] >> vTmp[18])) { break;}
		rawDATA2.push_back(vTmp);
	}
	DOSYA.close();
	/***********************************************************/
	numData=(int)rawDATA2.size();
	ALLOC_MEM(numData);
	for (int i = 0; i < numData; i++) {
		ZAMAN[i]		= rawDATA1[i][0];
		price_open[i]	= rawDATA1[i][1];
		price_high[i]	= rawDATA1[i][2];
		price_low[i]	= rawDATA1[i][3];
		price_close[i]	= rawDATA1[i][4];
		price_volume[i]	= rawDATA1[i][5];

		LoutSMA_25[i]	=rawDATA2[i][0];
		LoutSMA_99[i]	=rawDATA2[i][1];
		LoutEMA[i]		=rawDATA2[i][2];
		LoutSTD[i]		=rawDATA2[i][3];

		LoutUpperBand[i]	=rawDATA2[i][4];
		LoutMiddleBand[i]	=rawDATA2[i][5];
		LoutLowerBand[i]	=rawDATA2[i][6];

		LoutRSI[i]		=rawDATA2[i][7];
		LoutARN[i]		=rawDATA2[i][8];
		LoutARN_up[i]	=rawDATA2[i][9];
		LoutARN_down[i]	=rawDATA2[i][10];

		LoutMACD[i]			=rawDATA2[i][11];
		LoutMACDSignal[i]	=rawDATA2[i][12];
		LoutMACDHist[i]		=rawDATA2[i][13];
		LoutFastK[i]		=rawDATA2[i][14];
		LoutFastD[i]		=rawDATA2[i][15];
		LoutADX[i]			=rawDATA2[i][16];
		LoutDIp[i]			=rawDATA2[i][17];
		LoutDIn[i]			=rawDATA2[i][18];
	}
	rawDATA1.clear();
	rawDATA2.clear();
	getStart(valData);
	return;

	for (int i = 0; i < numData; i++) {
		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<i;
		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<LoutSMA_25[i];
		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<LoutSMA_99[i];
		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<LoutEMA[i];
		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<LoutSTD[i];

		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<LoutUpperBand[i];
		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<LoutMiddleBand[i];
		std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left ; std::cout<<LoutLowerBand[i];

		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutRSI[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutARN[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutARN_up[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutARN_down[i];

		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutMACD[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutMACDSignal[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutMACDHist[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutFastK[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutFastD[i];
		std::cout.fill(' '); std::cout.width(6); 	std::cout << std::left ; std::cout<<LoutADX[i]<<"\n";
	}
}
/**************************************************************************************************************/
void INDICATOR::CALC() {
	int startIdx = 0;
	int endIdx = numData - 1;
	int outBegIdx = 0;
	int outNbElement = 0;

	int DONEM_SMA_25 = 25;
	int DONEM_SMA_99 = 99;
	int DONEM_EMA = 99;
	int DONEM_STD = 20;

	int DONEM_RSI = 14;
	int DONEM_ARN = 25;
	int DONEM_ADX = 14;

	int MACFastPeriod = 12;
	int MACSlowPeriod = 26;
	int MACSignalPeriod = 9;

	int optInTimePeriod = 14;
	int optInFastK_Period = 3;
	int optInFastD_Period = 3;

	int DONEM_BBA = 20;
	double optInNbDevUp = 2;
	double optInNbDevDn = 2;

	double optInNbDev=1;

	TA_RetCode code;
	code=TA_SMA (startIdx, endIdx, &price_close[0], DONEM_SMA_25, &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < outNbElement; j++) { LoutSMA_25[j+outBegIdx] = out1[j]; }
	code=TA_SMA (startIdx, endIdx, &price_close[0], DONEM_SMA_99, &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < outNbElement; j++) { LoutSMA_99[j+outBegIdx] = out1[j]; }
	code=TA_EMA (startIdx, endIdx, &price_close[0], DONEM_EMA   , &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < outNbElement; j++) { LoutEMA[j+outBegIdx] = out1[j]; }
	code=TA_STDDEV(startIdx, endIdx, &price_close[0], DONEM_STD, optInNbDev, &outBegIdx, &outNbElement,&out1[0]); for (int j = 0; j < outNbElement; j++) { LoutSTD[j+outBegIdx] = out1[j]; }

	code=TA_BBANDS(startIdx, endIdx, &price_close[0], DONEM_BBA, optInNbDevUp, optInNbDevDn, TA_MAType_SMA, &outBegIdx, &outNbElement, &out1[0], &out2[0], &out3[0]);
	for (int j = 0; j < outNbElement; j++) {
		LoutUpperBand[j+outBegIdx] = out1[j];
		LoutMiddleBand[j+outBegIdx] = out2[j];
		LoutLowerBand[j+outBegIdx] = out3[j];
	}	
	code=TA_RSI     (startIdx, endIdx, &price_close[0], DONEM_RSI   , &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < outNbElement; j++) { LoutRSI[j+outBegIdx] = out1[j]; }
	code=TA_AROONOSC(startIdx, endIdx, &price_high[0], &price_low[0], DONEM_ARN, &outBegIdx, &outNbElement, &out1[0]);
	code=TA_AROON   (startIdx, endIdx, &price_high[0], &price_low[0], DONEM_ARN, &outBegIdx, &outNbElement, &out2[0], &out3[0]);

	for (int j = 0; j < outNbElement; j++) {
		LoutARN[j+outBegIdx] = out1[j];
		LoutARN_down[j+outBegIdx] = out2[j];
		LoutARN_up  [j+outBegIdx] = out3[j];
	}

	code=TA_MACD(startIdx, endIdx, &price_close[0], MACFastPeriod, MACSlowPeriod, MACSignalPeriod, &outBegIdx, &outNbElement, &out1[0], &out2[0], &out3[0]);
	for (int j = 0; j < outNbElement; j++) {
		LoutMACD[j+outBegIdx] = out1[j];
		LoutMACDSignal[j+outBegIdx] = out2[j];
		LoutMACDHist[j+outBegIdx] = out3[j];		
	}
	/********************************************/
	code=TA_STOCHRSI(startIdx, endIdx, &price_close[0], optInTimePeriod, optInFastK_Period, optInFastD_Period, TA_MAType_SMA, &outBegIdx, &outNbElement, &out1[0], &out2[0]);
//	code=TA_RSI(startIdx, endIdx, &price_close[0], 14, &outBegIdx, &outNbElement, &out3[0]);	
//	startIdx = outBegIdx;
//	endIdx = outNbElement-1;
//	code=TA_STOCH(startIdx, endIdx, &out3[0], &out3[0], &out3[0], 14, 3, TA_MAType_SMA, 3, TA_MAType_SMA, &outBegIdx, &outNbElement, &out1[0], &out2[0]);
	startIdx = 0;
	endIdx = numData - 1;
	code=TA_STOCH(startIdx, endIdx, &LoutRSI[0], &LoutRSI[0], &LoutRSI[0], 14, 3, TA_MAType_SMA, 3, TA_MAType_SMA, &outBegIdx, &outNbElement, &out1[0], &out2[0]);
	for (int j = 0; j < outNbElement; j++) {
		LoutFastK[j+outBegIdx] = out1[j];
		LoutFastD[j+outBegIdx] = out2[j];
	}
	code=TA_ADX     (startIdx, endIdx, &price_high[0], &price_low[0], &price_close[0], DONEM_ADX, &outBegIdx, &outNbElement, &out1[0]);	for (int j = 0; j < outNbElement; j++) { LoutADX[j+outBegIdx] = out1[j]; }
	code=TA_PLUS_DI (startIdx, endIdx, &price_high[0], &price_low[0], &price_close[0], DONEM_ADX, &outBegIdx, &outNbElement, &out1[0]);	for (int j = 0; j < outNbElement; j++) { LoutDIp[j+outBegIdx] = out1[j]; }
	code=TA_MINUS_DI(startIdx, endIdx, &price_high[0], &price_low[0], &price_close[0], DONEM_ADX, &outBegIdx, &outNbElement, &out1[0]);	for (int j = 0; j < outNbElement; j++) { LoutDIn[j+outBegIdx] = out1[j]; }
}
/**************************************************************************************************************/
void INDICATOR::DEALLOC_MEM(){
	if(!price_open  ){delete []price_open;}
	if(!price_high  ){delete []price_high;}
	if(!price_low   ){delete []price_low;}
	if(!price_close ){delete []price_close;}
	if(!price_volume){delete []price_volume;}

	if(!LoutSMA_25  ){delete []LoutSMA_25;}
	if(!LoutSMA_99  ){delete []LoutSMA_99;}
	if(!LoutEMA     ){delete []LoutEMA;}
	if(!LoutSTD     ){delete []LoutSTD;}

	if(!LoutUpperBand ){delete []LoutUpperBand;}
	if(!LoutMiddleBand){delete []LoutMiddleBand;}
	if(!LoutLowerBand ){delete []LoutLowerBand;}

	if(!LoutRSI     ){delete []LoutRSI;}
	if(!LoutARN     ){delete []LoutARN;}
	if(!LoutARN_up  ){delete []LoutARN_up;}
	if(!LoutARN_down){delete []LoutARN_down;}

	if(!LoutMACD ){delete []LoutMACD;}
	if(!LoutMACDSignal){delete []LoutMACDSignal;}
	if(!LoutMACDHist ){delete []LoutMACDHist;}
	if(!LoutFastK ){delete []LoutFastK;}
	if(!LoutFastD){delete []LoutFastD;}
	if(!LoutADX ){delete []LoutADX;}

	if(!LoutDate ){delete []LoutDate;}
	if(!ZAMAN ){delete []ZAMAN;}

	if(!out1){delete []out1;}
	if(!out2){delete []out2;}
	if(!out3){delete []out3;}

	price_open  =NULL;
	price_high  =NULL;
	price_low   =NULL;
	price_close =NULL;
	price_volume=NULL;

	LoutSMA_25  =NULL;
	LoutSMA_99  =NULL;
	LoutEMA     =NULL;
	LoutSTD     =NULL;

	LoutUpperBand  = NULL;
	LoutMiddleBand = NULL;
	LoutLowerBand  = NULL;

	LoutRSI     = NULL;
	LoutARN     = NULL;
	LoutARN_up  = NULL;
	LoutARN_down= NULL;

	LoutMACD		= NULL;
	LoutMACDSignal	= NULL;
	LoutMACDHist	= NULL;
	LoutFastK		= NULL;
	LoutFastD		= NULL;
	LoutADX			= NULL;
	LoutDIp			= NULL;
	LoutDIn			= NULL;

	LoutDate		= NULL;
	ZAMAN			= NULL;
	out1			= NULL;
	out2			= NULL;
	out3			= NULL;
}
/**************************************************************************************************************/
void INDICATOR::ALLOC_MEM(long long  _numData) {
	DEALLOC_MEM();
	price_open		=new double[_numData];
	price_high		=new double[_numData];
	price_low		=new double[_numData];
	price_close		=new double[_numData];
	price_volume	=new double[_numData];

	LoutSMA_25		=new double[_numData];
	LoutSMA_99		=new double[_numData];
	LoutEMA			=new double[_numData];
	LoutSTD			=new double[_numData];

	LoutUpperBand	= new double[_numData];
	LoutMiddleBand	= new double[_numData];
	LoutLowerBand	= new double[_numData];

	LoutRSI			= new double[_numData];
	LoutARN			= new double[_numData];
	LoutARN_up		= new double[_numData];
	LoutARN_down	= new double[_numData];

	LoutMACD		= new double[_numData];
	LoutMACDSignal	= new double[_numData];
	LoutMACDHist	= new double[_numData];
	LoutFastK		= new double[_numData];
	LoutFastD		= new double[_numData];
	LoutADX			= new double[_numData];
	LoutDIp			= new double[_numData];
	LoutDIn			= new double[_numData];

	LoutDate		= new std::string[_numData];
	ZAMAN			= new long long int[_numData];
	out1			= new double[_numData];
	out2			= new double[_numData];
	out3			= new double[_numData];

	for (int i = 0; i < _numData; i++) {
		price_open  [i] = 0;
		price_high  [i] = 0;
		price_low   [i] = 0;
		price_close [i] = 0;
		price_volume[i] = 0;

		LoutSMA_25[i] = 0;
		LoutSMA_99[i] = 0;
		LoutEMA   [i] = 0;
		LoutSTD   [i] = 0;

		LoutUpperBand [i] = 0;
		LoutMiddleBand[i] = 0;
		LoutLowerBand [i] = 0;

		LoutRSI     [i] = 0;
		LoutARN     [i] = 0;
		LoutARN_up  [i] = 0;
		LoutARN_down[i] = 0;

		LoutMACD[i] = 0;
		LoutMACDSignal[i] = 0;
		LoutMACDHist[i] = 0;
		LoutFastK[i] = 0;
		LoutFastD[i] = 0;
		LoutADX[i] = 0;
		LoutDIp[i] = 0;
		LoutDIn[i] = 0;

		LoutDate [i] = ' ';
		ZAMAN[i]=0;

		out1[i] = 0;
		out2[i] = 0;
		out3[i] = 0;
	}
}
/**************************************************************************************************************/
/**************************************************************************************************************/
void INDICATOR::getStart(int &_valData) {
	for (int i = 0; i < numData; i++) {
		if(
//		price_open[i]	==0 ||
//		price_high[i]	==0 ||
//		price_low[i]	==0 ||
//		price_close[i]	==0 ||
//		price_volume[i]	==0 ||

		LoutSMA_25[i]	==0 ||
		LoutSMA_99[i]	==0 ||
		LoutEMA[i]		==0 ||
		LoutSTD[i]		==0 ||

		LoutUpperBand[i]	==0 ||
		LoutMiddleBand[i]	==0 ||
		LoutLowerBand[i]	==0 ||

		LoutRSI[i]		==0 ||
		LoutARN[i]		==0 ||
		LoutARN_up[i]	==0 ||
		LoutARN_down[i]	==0 ||

		LoutMACD[i]			==0 ||
		LoutMACDSignal[i]	==0 ||
		LoutMACDHist[i]		==0 ||
		LoutFastK[i]		==0 ||
		LoutFastD[i]		==0 ||
		LoutADX[i]			==0 ||
		LoutDIp[i]			==0 ||
		LoutDIn[i]			==0 ){continue;}
		_valData=i;
		return;
	}
}