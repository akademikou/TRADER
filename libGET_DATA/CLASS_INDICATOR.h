#pragma once
#ifndef CLASS_INDICATOR_H
#define CLASS_INDICATOR_H

#define CLASS_INDICATOR_API __declspec(dllexport)
/**************************************************************************************************************/
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "json.h"
class CLASS_INDICATOR_API INDICATOR {
public:
	long long  numData;
	int valData;

	double(*price_open);
	double(*price_high);
	double(*price_low);
	double(*price_close);
	double(*price_volume);

	double(*LoutSMA_25);
	double(*LoutSMA_99);
	double(*LoutEMA);
	double(*LoutSTD);

	double(*LoutUpperBand);
	double(*LoutMiddleBand);
	double(*LoutLowerBand);

	double(*LoutRSI);
	double(*LoutARN);
	double(*LoutARN_up);
	double(*LoutARN_down);

	double(*LoutMACD);
	double(*LoutMACDSignal);
	double(*LoutMACDHist);
	double(*LoutFastK);
	double(*LoutFastD);
	double(*LoutADX);
	double(*LoutDIp);	
	double(*LoutDIn);	
	
	long long *ZAMAN;
	std::string (*LoutDate);
	std::string name;
protected:
	std::string interval;	
	double* out1;
	double* out2;
	double* out3;
	
	BinaCPP MY_CONNECTION;
	Json::Value MY_result;
public:
	virtual ~INDICATOR();
	INDICATOR();
	INDICATOR(std::string _name, std::string _interval, long long  _numData);
	void getStart(int &_valData);
	void DEALLOC_MEM();
	void ALLOC_MEM(long long  _numData);	
	void GET_DATA();
	void YAZ_DATA();
	void OKU_DATA();
	void CALC();
};
/**************************************************************************************************************/
#endif