#pragma once
#include <iostream>
#include <vector>
#include "ta_libc.h"
/**************************************************************************************************************/
class INDICATOR {
public:
	
	double(*LoutSMA_25);
	double(*LoutSMA_99);
	double(*LoutEMA);
	double(*LoutKMA);
	double(*LoutRSI);
	double(*LoutARN);
	double(*LoutARN_up);
	double(*LoutARN_down);	

	double(*LoutMACD);
	double(*LoutMACDSignal);
	double(*LoutMACDHist);
	double(*LoutFastK);
	double(*LoutFastD);
	double(*LoutUpperBand);
	double(*LoutMiddleBand);
	double(*LoutLowerBand);

	double(*LoutPrice_close);
	double(*LoutADX);
	bool NEW_SECTION;
	std::string name;
protected:
	
	std::string interval;
	int numData;
	int nLAST;
	
	double* out1;
	double* out2;
	double* out3;

	double(*price_open);
	double(*price_high);
	double(*price_low);
	double(*price_close);
	double(*price_volume);

	double old_0;
	double old_1;
	double old_2;

	double RSI_MAX, RSI_MIN;
	double ARN_MAX, ARN_MIN;	
	double MAC_MAX, MAC_MIN;
	double STC_MAX, STC_MIN;

public:
	virtual ~INDICATOR();
	INDICATOR();
	INDICATOR(std::string _name, std::string _interval, int _nLAST, int _numData);

	void ALLOC_MEM(int _nLAST, int _numData);
	void DEALLOC_MEM();
	void GET_DATA();
	void CALC();
	void PRINT(TRADER* _TRADER);
	void STRATEGY_ALL(TRADER* _TRADER);
	
	void STRATEGY_MEAN(TRADER* _TRADER);
	void STRATEGY_CROS(TRADER* _TRADER);
	void STRATEGY_ONC(TRADER* _TRADER);
	void STRATEGY_RSI(TRADER* _TRADER);
	void STRATEGY_ARN(TRADER* _TRADER);
	void STRATEGY_MAC(TRADER* _TRADER);
	void STRATEGY_STC(TRADER* _TRADER);
};
/**************************************************************************************************************/
INDICATOR::~INDICATOR() {}
/**************************************************************************************************************/
INDICATOR::INDICATOR(){	DEALLOC_MEM();}
/**************************************************************************************************************/
INDICATOR::INDICATOR(std::string _name, std::string _interval,int _nLAST, int _numData) {
	name = _name;
	interval = _interval;
	numData = _numData;
	nLAST = _nLAST;
	ALLOC_MEM(_nLAST, _numData);
	
	RSI_MAX =  80;
	RSI_MIN =  20;
	ARN_MAX =  80;
	ARN_MIN = -80;
	MAC_MAX =  0;
	MAC_MIN =  0;
	STC_MAX =  85;
	STC_MIN =  15;
}
/**************************************************************************************************************/

void INDICATOR::GET_DATA() {	
	MY_CONNECTION.get_klines(name.c_str(), interval.c_str(), numData, 0, 0, MY_result);
	for (unsigned int j = 0; j < MY_result.size(); j++) {
		price_open[j] = std::stod(MY_result[j][1].asString());
		price_high[j] = std::stod(MY_result[j][2].asString());
		price_low[j] = std::stod(MY_result[j][3].asString());
		price_close[j] = std::stod(MY_result[j][4].asString());
		price_volume[j] = std::stod(MY_result[j][5].asString());
	}
	
	old_0 = LoutPrice_close[0];
	old_1 = LoutPrice_close[1];
	old_2 = LoutPrice_close[2];

	for (int j = 0; j < nLAST; j++) {
		LoutPrice_close[j] = price_close[(numData - 1) - j];
	}

	if (old_1 != LoutPrice_close[1] || old_2 != LoutPrice_close[2])
	{NEW_SECTION = true;}
	else { NEW_SECTION = false; }
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
	int DONEM_KMA = 99;

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


	TA_ADX (startIdx, endIdx, &price_high[0], &price_low[0], &price_close[0], DONEM_ADX, &outBegIdx, &outNbElement, &out1[0]); 
	for (int j = 0; j < nLAST; j++) { LoutADX[j] = out1[outNbElement - 1 - j]; }
	

	TA_SMA (startIdx, endIdx, &price_close[0], DONEM_SMA_25, &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < nLAST; j++) { LoutSMA_25[j] = out1[outNbElement - 1 - j]; }
	TA_SMA (startIdx, endIdx, &price_close[0], DONEM_SMA_99, &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < nLAST; j++) { LoutSMA_99[j] = out1[outNbElement - 1 - j]; }
	TA_EMA (startIdx, endIdx, &price_close[0], DONEM_EMA   , &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < nLAST; j++) { LoutEMA[j] = out1[outNbElement - 1 - j]; }
	TA_KAMA(startIdx, endIdx, &price_close[0], DONEM_KMA   , &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < nLAST; j++) { LoutKMA[j] = out1[outNbElement - 1 - j]; }
	TA_RSI (startIdx, endIdx, &price_close[0], DONEM_RSI   , &outBegIdx, &outNbElement, &out1[0]); for (int j = 0; j < nLAST; j++) { LoutRSI[j] = out1[outNbElement - 1 - j]; }

	TA_AROONOSC(startIdx, endIdx, &price_high[0], &price_low[0], DONEM_ARN, &outBegIdx, &outNbElement, &out1[0]);
	TA_AROON   (startIdx, endIdx, &price_high[0], &price_low[0], DONEM_ARN, &outBegIdx, &outNbElement, &out2[0], &out3[0]);

	for (int j = 0; j < nLAST; j++) {
		LoutARN[j] = out1[outNbElement - 1 - j];
		LoutARN_down[j] = out2[outNbElement - 1 - j];
		LoutARN_up  [j] = out3[outNbElement - 1 - j];
	}

	TA_MACD(startIdx, endIdx, &price_close[0], MACFastPeriod, MACSlowPeriod, MACSignalPeriod, &outBegIdx, &outNbElement, &out1[0], &out2[0], &out3[0]);
	for (int j = 0; j < nLAST; j++) {
		LoutMACD[j] = out1[outNbElement - 1 - j];
		LoutMACDSignal[j] = out2[outNbElement - 1 - j];
		LoutMACDHist[j] = out3[outNbElement - 1 - j];
	}
	/********************************************/
	TA_STOCHRSI(startIdx, endIdx, &price_close[0], optInTimePeriod, optInFastK_Period, optInFastD_Period, TA_MAType_SMA, &outBegIdx, &outNbElement, &out1[0], &out2[0]);
	TA_RSI(startIdx, endIdx, &price_close[0], 14, &outBegIdx, &outNbElement, &out3[0]);	
	startIdx = outBegIdx;
	endIdx = outNbElement-1;
	TA_STOCH(startIdx, endIdx, &out3[0], &out3[0], &out3[0], 14, 3, TA_MAType_SMA, 3, TA_MAType_SMA, &outBegIdx, &outNbElement, &out1[0], &out2[0]);
	for (int j = 0; j < nLAST; j++) {
		LoutFastK[j] = out1[outNbElement - 1 - j];
		LoutFastD[j] = out2[outNbElement - 1 - j];
	}
	/********************************************/
	TA_BBANDS(startIdx, endIdx, &price_close[0], DONEM_BBA, optInNbDevUp, optInNbDevDn, TA_MAType_SMA, &outBegIdx, &outNbElement, &out1[0], &out2[0], &out3[0]);
	for (int j = 0; j < nLAST; j++) {
		LoutUpperBand[j] = out1[outNbElement - 1 - j];
		LoutMiddleBand[j] = out2[outNbElement - 1 - j];
		LoutLowerBand[j] = out3[outNbElement - 1 - j];
	}
}
/**************************************************************************************************************/

void INDICATOR::STRATEGY_MEAN(TRADER* _TRADER) {
	// Ortalamadan %2 küçük ise al, %2 büyük ise sat
	double ORAN = 0.02;
	double last_price = LoutPrice_close[0];
	double SMA_25_0 = LoutSMA_25[0];
	double SMA_25_L = SMA_25_0 * (1 - ORAN);
	double SMA_25_H = SMA_25_0 * (1 + ORAN);
		 if (last_price < SMA_25_L) { _TRADER->SMA_25 =  1; }
	else if (last_price > SMA_25_H) { _TRADER->SMA_25 = -1; }
	else { _TRADER->SMA_25 = 0; }

	double SMA_99_0 = LoutSMA_99[0];
	double SMA_99_L = SMA_99_0 * (1 - ORAN);
	double SMA_99_H = SMA_99_0 * (1 + ORAN);
		 if (last_price < SMA_99_L) { _TRADER->SMA_99 =  1; }
	else if (last_price > SMA_99_H) { _TRADER->SMA_99 = -1; }
	else { _TRADER->SMA_99 = 0; }

	double EMA_0 = LoutEMA[0];
	double EMA_L = EMA_0 * (1 - ORAN);
	double EMA_H = EMA_0 * (1 + ORAN);
		 if (last_price < EMA_L) { _TRADER->EMA =  1; }
	else if (last_price > EMA_H) { _TRADER->EMA = -1; }
	else { _TRADER->EMA = 0; }

	double KMA_0 = LoutKMA[0];
	double KMA_L = KMA_0 * (1 - ORAN);
	double KMA_H = KMA_0 * (1 + ORAN);
		 if (last_price < KMA_L) { _TRADER->KMA =  1; }
	else if (last_price > KMA_H) { _TRADER->KMA = -1; }
	else { _TRADER->KMA = 0; }
}
/**************************************************************************************************************/
void INDICATOR::STRATEGY_CROS(TRADER* _TRADER) {
	double prev_price = LoutPrice_close[1];
	double prev_sma_25 = LoutSMA_25[1];
	double prev_sma_99 = LoutSMA_99[1];
	double last_price = LoutPrice_close[0];
	double last_sma_25 = LoutSMA_25[0];
	double last_sma_99 = LoutSMA_99[0];
	// current price crosses sma(cross upwards = buy, downwards = sell)	
		 if (last_price > last_sma_99 && prev_price < last_sma_99) { _TRADER->SMA =  1; }
	else if (last_price < last_sma_99 && prev_price > last_sma_99) { _TRADER->SMA = -1; }
	else { _TRADER->SMA = 0; }
	// sma25 crosses sma99(cross upwards = buy, downwards = sell)	
		 if (last_sma_25 > last_sma_99 and prev_sma_25 < prev_sma_99) { _TRADER->TRG =  1; }
	else if (last_sma_25 < last_sma_99 and prev_sma_25 > prev_sma_99) { _TRADER->TRG = -1; }
	else { _TRADER->TRG = 0; }
	 // current price crosses bollinger up (buy), crosses bollingerdown downwards (sell)
	static int trig = 0;
	double ORAN = 0.001;
	double BOL_M = LoutMiddleBand[0];
	double BOL_L = LoutLowerBand[0] * (1 - ORAN);
	double BOL_H = LoutUpperBand[0] * (1 + ORAN);
	double last_arn_up = LoutARN_up[0];
	double last_arn_down = LoutARN_down[0];

		 if (last_price < BOL_L) { trig =  1; }
	else if (last_price > BOL_H) { trig = -1; }

	if (trig ==  1 && last_price > BOL_M) { trig = 0; }
	if (trig == -1 && last_price < BOL_M) { trig = 0; }
	_TRADER->BOL = 0;
		 if (last_price > last_sma_25 && trig ==  1) { _TRADER->BOL =  1; trig = 0; }
	else if (last_price < last_sma_25 && trig == -1) { _TRADER->BOL = -1; trig = 0; }
		 if (last_arn_down < ARN_MAX && trig ==  1) { _TRADER->BOL =  1; trig = 0; }	// AL
	else if (last_arn_up   < ARN_MAX && trig == -1) { _TRADER->BOL = -1; trig = 0; }	// SAT
}
/**************************************************************************************************************/
void INDICATOR::STRATEGY_ONC(TRADER* _TRADER) {
	double ORAN = 0.015;
	double last_price = LoutPrice_close[0];
	double MARG_L = _TRADER->LAST_SEL * (1 - ORAN);
	double MARG_H = _TRADER->LAST_BUY * (1 + ORAN);
	if (_TRADER->LAST_SEL != 0 && last_price < MARG_L) { _TRADER->ONC = 1; }	// AL
	else if (_TRADER->LAST_BUY != 0 && last_price > MARG_H) { _TRADER->ONC = -1; }	// SAT
	else { _TRADER->ONC = 0; }
}
/**************************************************************************************************************/
void INDICATOR::STRATEGY_RSI(TRADER* _TRADER) {
	static int trig = 0;
	double prev_rsi = LoutRSI[1];
	double last_rsi = LoutRSI[0];
	if (last_rsi < RSI_MIN && prev_rsi > RSI_MIN) { trig =  1; }	// cross down
	if (last_rsi > RSI_MAX && prev_rsi < RSI_MAX) { trig = -1; }	// cross up

		 if (last_rsi > RSI_MIN && trig ==  1) { _TRADER->RSI =  1; trig = 0;}	// AL
	else if (last_rsi < RSI_MAX && trig == -1) { _TRADER->RSI = -1; trig = 0;}	// SAT
	else { _TRADER->RSI = 0; }
}
/**************************************************************************************************************/
void INDICATOR::STRATEGY_ARN(TRADER* _TRADER) {
	// UÇ değerlerin bu ana olan yakınlığını ölçer (UP-DOWN)
	static int trig = 0;
	double prev_arn = LoutARN[1];
	double prev_arn_up = LoutARN_up[1];
	double prev_arn_down = LoutARN_down[1];

	double last_arn = LoutARN[0];
	double last_arn_up = LoutARN_up[0];
	double last_arn_down = LoutARN_down[0];	

	if (last_arn > 0 && prev_arn < 0) { trig =  1; }	// cross up
	if (last_arn < 0 && prev_arn > 0) { trig = -1; }	// cross down

	if (last_arn_up > last_arn_down && prev_arn_up < prev_arn_down) { trig = 1; }	// cross up
	if (last_arn_up < last_arn_down && prev_arn_up > prev_arn_down) { trig = -1; }	// cross down


		 if (last_arn_up   > ARN_MAX && last_arn_down < 50 && trig ==  1) { _TRADER->ARN =  1; trig = 0;}	// AL
	else if (last_arn_down > ARN_MAX && last_arn_up   < 50 && trig == -1) { _TRADER->ARN = -1; trig = 0;}	// SAT
	else { _TRADER->ARN = 0; }
}
/**************************************************************************************************************/
void INDICATOR::STRATEGY_MAC(TRADER* _TRADER) {

	static int trig = 0;
	double last_mac = LoutMACD[0];
	double prev_mac = LoutMACD[1];
	double last_mac_signal = LoutMACDSignal[0];
	double prev_mac_signal = LoutMACDSignal[1];
	bool S_U = (last_mac > last_mac_signal && prev_mac < prev_mac_signal);
	bool S_D = (last_mac < last_mac_signal && prev_mac > prev_mac_signal);

		 if (S_U == true && last_mac > 0) { trig =  1; }
	else if (S_D == true && last_mac < 0) { trig = -1; }
	else { _TRADER->MAC = 0; }
}
/**************************************************************************************************************/
void INDICATOR::STRATEGY_STC(TRADER* _TRADER) {
	static int trig = 0;
	double prev_stc_K = LoutFastK[1];
	double prev_stc_D = LoutFastD[1];
	double last_stc_K = LoutFastK[0];
	double last_stc_D = LoutFastD[0];
	if (prev_stc_K < prev_stc_D && last_stc_K > last_stc_D) { trig =  1; }	// cross up
	if (prev_stc_K > prev_stc_D && last_stc_K < last_stc_D) { trig = -1; }	// cross down
		 if (last_stc_K > STC_MIN && last_stc_D > STC_MIN && trig ==  1) {_TRADER->STC =  1; trig = 0;}	// AL
	else if (last_stc_K > STC_MAX && last_stc_D > STC_MAX && trig == -1) {_TRADER->STC = -1; trig = 0;}	// SAT
	else { _TRADER->STC = 0; }
}
/**************************************************************************************************************/
void INDICATOR::STRATEGY_ALL(TRADER* _TRADER) {
	if (LoutMACDHist[0] > LoutMACDHist[1] && LoutMACDHist[1] > LoutMACDHist[2]) { _TRADER->YON = 1; }
	else if (LoutMACDHist[0] < LoutMACDHist[1] && LoutMACDHist[1] < LoutMACDHist[2]) { _TRADER->YON = -1; }
	else { _TRADER->YON = 0; }

	if (LoutARN[0] > 70) {_TRADER->YON = 1;	}
	if (LoutARN[0] < -70) { _TRADER->YON = -1; }

	if (LoutADX[0] > 35) { return; }
	STRATEGY_MEAN(_TRADER);
	STRATEGY_CROS(_TRADER);
	STRATEGY_ONC(_TRADER);
	STRATEGY_RSI(_TRADER);
	STRATEGY_ARN(_TRADER);
	STRATEGY_MAC(_TRADER);
	STRATEGY_STC(_TRADER);



	if (LoutRSI[0] <60 && LoutPrice_close[0]>LoutMiddleBand[0]) { // satış için talep olgunlaşmadı
		if (_TRADER->SMA_25 == -1) { _TRADER->SMA_25 = 0; }
		if (_TRADER->SMA_99 == -1) { _TRADER->SMA_99 = 0; }
		if (_TRADER->EMA == -1) { _TRADER->EMA = 0; }
		if (_TRADER->KMA == -1) { _TRADER->KMA = 0; }
		
		if (_TRADER->SMA == -1) { _TRADER->SMA = 0; }
		if (_TRADER->TRG == -1) { _TRADER->TRG = 0; }
		if (_TRADER->BOL == -1) { _TRADER->BOL = 0; }
		if (_TRADER->ONC == -1) { _TRADER->ONC = 0; }

		if (_TRADER->RSI == -1) { _TRADER->RSI = 0; }
		if (_TRADER->ARN == -1) { _TRADER->ARN = 0; }
		if (_TRADER->MAC == -1) { _TRADER->MAC = 0; }
		if (_TRADER->STC == -1) { _TRADER->STC = 0; }
	}
	if (LoutRSI[0] > 40 && LoutPrice_close[0] < LoutMiddleBand[0]) { // alış için talep olgunlaşmadı
		if (_TRADER->SMA_25 == 1) { _TRADER->SMA_25 = 0; }
		if (_TRADER->SMA_99 == 1) { _TRADER->SMA_99 = 0; }
		if (_TRADER->EMA == 1) { _TRADER->EMA = 0; }
		if (_TRADER->KMA == 1) { _TRADER->KMA = 0; }
		
		if (_TRADER->SMA == 1) { _TRADER->SMA = 0; }
		if (_TRADER->TRG == 1) { _TRADER->TRG = 0; }
		if (_TRADER->BOL == 1) { _TRADER->BOL = 0; }
		if (_TRADER->ONC == 1) { _TRADER->ONC = 0; }

		if (_TRADER->RSI == 1) { _TRADER->RSI = 0; }
		if (_TRADER->ARN == 1) { _TRADER->ARN = 0; }
		if (_TRADER->MAC == 1) { _TRADER->MAC = 0; }
		if (_TRADER->STC == 1) { _TRADER->STC = 0; }
	}
}
/**************************************************************************************************************/
void INDICATOR::PRINT(TRADER* _TRADER) {
	std::string K[16];
	for (int i = 0; i < 16; i++) { K[i] = YELLOW; }

	if (_TRADER->SMA_25 > 0) { K[0] = BOLDGREEN; }
	else if (_TRADER->SMA_25 < 0) { K[0] = BOLDRED; }
	if (_TRADER->SMA_99 > 0) { K[1] = BOLDGREEN; }
	else if (_TRADER->SMA_99 < 0) { K[1] = BOLDRED; }
	if (_TRADER->EMA > 0) { K[2] = BOLDGREEN; }
	else if (_TRADER->EMA < 0) { K[2] = BOLDRED; }
	if (_TRADER->KMA > 0) { K[3] = BOLDGREEN; }
	else if (_TRADER->KMA < 0) { K[3] = BOLDRED; }

	if (_TRADER->SMA > 0) { K[4] = BOLDGREEN; }
	else if (_TRADER->SMA < 0) { K[4] = BOLDRED; }
	if (_TRADER->TRG > 0) { K[5] = BOLDGREEN; }
	else if (_TRADER->TRG < 0) { K[5] = BOLDRED; }
	if (_TRADER->BOL > 0) { K[6] = BOLDGREEN; }
	else if (_TRADER->BOL < 0) { K[6] = BOLDRED; }
	if (_TRADER->ONC > 0) { K[7] = BOLDGREEN; }
	else if (_TRADER->ONC < 0) { K[7] = BOLDRED; }

	if (_TRADER->RSI > 0) { K[8] = BOLDGREEN; }
	else if (_TRADER->RSI < 0) { K[8] = BOLDRED; }
	if (_TRADER->ARN > 0) { K[9] = BOLDGREEN; }
	else if (_TRADER->ARN < 0) { K[9] = BOLDRED; }
	if (_TRADER->MAC > 0) { K[10] = BOLDGREEN; }
	else if (_TRADER->MAC < 0) { K[10] = BOLDRED; }
	if (_TRADER->STC > 0) { K[11] = BOLDGREEN; }
	else if (_TRADER->STC < 0) { K[11] = BOLDRED; }

	if (_TRADER->YON > 0) { K[12] = BOLDGREEN; }
	else if (_TRADER->YON < 0) { K[12] = BOLDRED; }
	if (_TRADER->EMIR > 0) { K[13] = BOLDGREEN; }
	else if (_TRADER->EMIR < 0) { K[13] = BOLDRED; }


	std::cout << BOLDYELLOW << "____________________________________________________________________________________________" << RESET << '\n';
	std::cout << YELLOW << name
		<< K[0] << " SMA_25 " << _TRADER->SMA_25
		<< K[1] << " SMA_99 " << _TRADER->SMA_99
		<< K[2] << " EMA " << _TRADER->EMA
		<< K[3] << " KMA " << _TRADER->KMA

		<< K[4] << " SMA " << _TRADER->SMA
		<< K[5] << " TRG " << _TRADER->TRG
		<< K[6] << " BOL " << _TRADER->BOL
		<< K[7] << " ONC " << _TRADER->ONC

		<< K[8] << " RSI " << _TRADER->RSI
		<< K[9] << " ARN " << _TRADER->ARN
		<< K[10] << " MAC " << _TRADER->MAC
		<< K[11] << " STC " << _TRADER->STC

		<< K[12] << " YON " << _TRADER->YON
		<< K[13] << " EMIR " << _TRADER->EMIR << "    " << RESET << '\n';

	std::cout << BOLDRED <<
		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "price" <<

		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "SMA_25" <<
		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "SMA_99" <<
		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "EMA" <<
		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "KMA" <<

		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "Upper_B" <<
		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "Middle_B" <<
		std::cout.fill(' '); std::cout.width(10); std::cout << std::right << "Lower_B" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "RSI" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "ARN" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "ARN_U" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "ARN_D" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "MACD" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "MACD_S" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "MACD_H" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "FastK" <<
		std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "FastD" << RESET << '\n';



	for (int i = 0; i < 16; i++) { K[i] = Back_BLACK; }

	for (int j = 0; j < nLAST; j++) {

		std::string K_B = BOLDCYAN;
		if (j == 0) {
			K_B = BOLDGREEN;
		}
		for (int i = 0; i < 16; i++) { K[i] = K_B; }

		// AL YESIL  SAT KIRMIZI
		double ORAN = 0.015;
		double last_price = LoutPrice_close[j];
		double SMA_25_0 = LoutSMA_25[j];
		double SMA_25_L = SMA_25_0 * (1 - ORAN);
		double SMA_25_H = SMA_25_0 * (1 + ORAN);
		double SMA_99_0 = LoutSMA_99[j];
		double SMA_99_L = SMA_99_0 * (1 - ORAN);
		double SMA_99_H = SMA_99_0 * (1 + ORAN);
		double EMA_0 = LoutEMA[j];
		double EMA_L = EMA_0 * (1 - ORAN);
		double EMA_H = EMA_0 * (1 + ORAN);
		double KMA_0 = LoutKMA[j];
		double KMA_L = KMA_0 * (1 - ORAN);
		double KMA_H = KMA_0 * (1 + ORAN);

		double BOL_H = LoutUpperBand[j] * (1 + 0.003);
		double BOL_M = LoutMiddleBand[j];
		double BOL_L = LoutLowerBand[j] * (1 - 0.003);


		if (last_price > SMA_25_H) { K[0] = Back_RED; }
		else if (last_price < SMA_25_L) { K[0] = Back_GREEN; }
		if (last_price > SMA_99_H) { K[1] = Back_RED; }
		else if (last_price < SMA_99_L) { K[1] = Back_GREEN; }
		if (last_price > EMA_H) { K[2] = Back_RED; }
		else if (last_price < EMA_L) { K[2] = Back_GREEN; }
		if (last_price > KMA_H) { K[3] = Back_RED; }
		else if (last_price < KMA_L) { K[3] = Back_GREEN; }

		if (last_price > BOL_H) { K[4] = Back_RED; }
		if (last_price > BOL_M) { K[5] = Back_RED; }
		else if (last_price < BOL_M) { K[5] = Back_GREEN; }
		if (last_price < BOL_L) { K[6] = Back_GREEN; }

		if (LoutRSI[j] > RSI_MAX) { K[7] = Back_RED; }
		else if (LoutRSI[j] < RSI_MIN) { K[7] = Back_GREEN; }
		if (LoutARN[j] > ARN_MAX) { K[8] = Back_RED; }
		else if (LoutARN[j] < ARN_MIN) { K[8] = Back_GREEN; }
		if (LoutARN_up[j] > ARN_MAX) { K[9] = Back_RED; }
		else if (LoutARN_up[j] < 50) { K[9] = Back_GREEN; }
		if (LoutARN_down[j] > ARN_MAX) { K[10] = Back_GREEN; }
		else if (LoutARN_down[j] < 50) { K[10] = Back_RED; }


		if (LoutMACD[j] > MAC_MAX) { K[11] = Back_RED; }
		else if (LoutMACD[j] < MAC_MIN) { K[11] = Back_GREEN; }
		if (_TRADER->MAC > 0) { K[12] = Back_RED; }
		else if (_TRADER->MAC < 0) { K[12] = Back_GREEN; }
		if (LoutMACDHist[j] > MAC_MAX) { K[13] = Back_RED; }
		else if (LoutMACDHist[j] < MAC_MIN) { K[13] = Back_GREEN; }

		if (LoutFastK[j] > STC_MAX) { K[14] = Back_RED; }
		else if (LoutFastK[j] < STC_MIN) { K[14] = Back_GREEN; }
		if (LoutFastD[j] > STC_MAX) { K[15] = Back_RED; }
		else if (LoutFastD[j] < STC_MIN) { K[15] = Back_GREEN; }


		std::cout << K_B <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutPrice_close[j] << RESET << K[0] <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutSMA_25[j] << RESET << K[1] <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutSMA_99[j] << RESET << K[2] <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutEMA[j] << RESET << K[3] <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutKMA[j] << RESET << K[4] <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutUpperBand[j] << RESET << K[5] <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutMiddleBand[j] << RESET << K[6] <<
			std::cout.fill(' '); std::cout.width(10); std::cout << std::right << LoutLowerBand[j] << RESET << K[7] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutRSI[j] << RESET << K[8] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutARN[j] << RESET << K[9] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutARN_up[j] << RESET << K[10] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutARN_down[j] << RESET << K[11] <<

			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutMACD[j] << RESET << K[12] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutMACDSignal[j] << RESET << K[13] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutMACDHist[j] << RESET << K[14] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutFastK[j] << RESET << K[15] <<
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutFastD[j] << RESET << 
			std::cout.fill(' '); std::cout.width(7); std::cout << std::right << LoutADX[j] << RESET << "  \n";
	}
}
/**************************************************************************************************************/
void INDICATOR::ALLOC_MEM(int _nLAST, int _numData) {
	LoutSMA_25 = new double[_nLAST];
	LoutSMA_99 = new double[_nLAST];
	LoutEMA = new double[_nLAST];
	LoutKMA = new double[_nLAST];
	LoutRSI = new double[_nLAST];
	LoutARN = new double[_nLAST];
	LoutARN_up = new double[_nLAST];
	LoutARN_down = new double[_nLAST];

	LoutMACD = new double[_nLAST];
	LoutMACDSignal = new double[_nLAST];
	LoutMACDHist = new double[_nLAST];
	LoutFastK = new double[_nLAST];
	LoutFastD = new double[_nLAST];
	LoutUpperBand = new double[_nLAST];
	LoutMiddleBand = new double[_nLAST];
	LoutLowerBand = new double[_nLAST];

	LoutPrice_close = new double[_nLAST];

	LoutADX = new double[_nLAST];

	for (int i = 0; i < _nLAST; i++) {
		LoutSMA_25[i] = 0;
		LoutSMA_99[i] = 0;
		LoutEMA[i] = 0;
		LoutKMA[i] = 0;
		LoutRSI[i] = 0;
		LoutARN[i] = 0;
		LoutARN_up[i] = 0;
		LoutARN_down[i] = 0;

		LoutMACD[i] = 0;
		LoutMACDSignal[i] = 0;
		LoutMACDHist[i] = 0;
		LoutFastK[i] = 0;
		LoutFastD[i] = 0;
		LoutUpperBand[i] = 0;
		LoutMiddleBand[i] = 0;
		LoutLowerBand[i] = 0;

		LoutPrice_close[i] = 0;
		LoutADX[i] = 0;
	}

	price_open = new double[_numData];
	price_high = new double[_numData];
	price_low = new double[_numData];
	price_close = new double[_numData];
	price_volume = new double[_numData];

	out1 = new double[_numData];
	out2 = new double[_numData];
	out3 = new double[_numData];

	for (int i = 0; i < _numData; i++) {
		price_open[i] = 0;
		price_high[i] = 0;
		price_low[i] = 0;
		price_close[i] = 0;
		price_volume[i] = 0;

		out1[i] = 0;
		out2[i] = 0;
		out3[i] = 0;
	}

}
/**************************************************************************************************************/
void INDICATOR::DEALLOC_MEM() {
	delete[] LoutSMA_25;
	delete[] LoutSMA_99;
	delete[] LoutEMA;
	delete[] LoutKMA;
	delete[] LoutRSI;
	delete[] LoutARN;

	delete[] LoutMACD;
	delete[] LoutMACDSignal;
	delete[] LoutMACDHist;
	delete[] LoutFastK;
	delete[] LoutFastD;
	delete[] LoutUpperBand;
	delete[] LoutMiddleBand;
	delete[] LoutLowerBand;

	delete[] LoutPrice_close;
	delete[] LoutADX;

	delete[] price_open;
	delete[] price_high;
	delete[] price_low;
	delete[] price_close;
	delete[] price_volume;

	delete[] out1;
	delete[] out2;
	delete[] out3;
}
/**************************************************************************************************************/