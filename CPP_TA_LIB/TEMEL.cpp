#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <assert.h>
#include "ta_libc.h"


/************************************************************************************/
void READ_TRAIN_SET(std::string _fNAME, std::vector<std::vector<double>>& _rawDATA) {
    std::fstream DOSYA;
    DOSYA.open(_fNAME.c_str(), std::ios::in);
    _rawDATA.clear();
    std::vector<double> vTmp(17);
    while (!DOSYA.eof()) {
        DOSYA >> vTmp[0]; // open
        DOSYA >> vTmp[1]; // high
        DOSYA >> vTmp[2]; // low
        DOSYA >> vTmp[3]; // close

        DOSYA >> vTmp[4]; // ama
        DOSYA >> vTmp[5]; // ema
        DOSYA >> vTmp[6]; // kma
        DOSYA >> vTmp[7]; // rsi
        DOSYA >> vTmp[8]; // arn
        DOSYA >> vTmp[9]; // macd
        DOSYA >> vTmp[10]; // macdsignal
        DOSYA >> vTmp[11]; // macdhist
        DOSYA >> vTmp[12]; // fastk
        DOSYA >> vTmp[13]; // fastd
        DOSYA >> vTmp[14]; // upperband
        DOSYA >> vTmp[15]; // middleband
        DOSYA >> vTmp[16]; // lowerband

        _rawDATA.push_back(vTmp);
    }
    DOSYA.close();
// open high low close sma ema kma rsi arn macd macdsignal macdhist fastk fastd upperband middleband lowerband
}
/************************************************************************************/
void main(){
    const std::string Fname = "BTCUSDT_candle.txt";
    std::vector<std::vector<double>> raw_DATA;    
    READ_TRAIN_SET(Fname, raw_DATA);

    int BOY = (int)raw_DATA.size();

    double* price_open = new double[BOY];
    double* price_high = new double[BOY];
    double* price_low = new double[BOY];
    double* price_close = new double[BOY];
    for (int i = 0; i < BOY; i++) {
        price_open[i] = raw_DATA[i][0];
        price_high[i] = raw_DATA[i][1];
        price_low[i] = raw_DATA[i][2];
        price_close[i] = raw_DATA[i][3];
    }

    int startIdx = 100;
    int endIdx = BOY - 1;
    int outBegIdx = 0;          // TA_Integer
    int outNbElement = 0;       // TA_Integer

    int DONEM_SMA = 99;
    int DONEM_EMA = 99;
    int DONEM_KMA = 30;
    int DONEM_RSI = 14;    
    int DONEM_ARN = 30;
    int DONEM_BBA=5;
    int optInFastPeriod = 12;
    int optInSlowPeriod = 26;
    int optInSignalPeriod = 9;

    int optInTimePeriod = 14;
    int optInFastK_Period = 3;
    int optInFastD_Period = 3;
    double optInNbDevUp = 2;
    double optInNbDevDn = 2;

    double* outSMA = new double[BOY]; // TA_Real
    double* outEMA = new double[BOY];
    double* outKMA = new double[BOY];
    double* outRSI = new double[BOY];
    double* outARN = new double[BOY];

    double* outMACD = new double[BOY];
    double* outMACDSignal = new double[BOY];
    double* outMACDHist = new double[BOY];
    double* outFastK = new double[BOY];
    double* outFastD = new double[BOY];
    double* outUpperBand = new double[BOY];
    double* outMiddleBand = new double[BOY];
    double* outLowerBand = new double[BOY];    

    TA_SMA (startIdx, endIdx, &price_close[0], DONEM_SMA, &outBegIdx, &outNbElement, &outSMA[0]);
    TA_EMA (startIdx, endIdx, &price_close[0], DONEM_EMA, &outBegIdx, &outNbElement, &outEMA[0]);
    TA_KAMA(startIdx, endIdx, &price_close[0], DONEM_KMA, &outBegIdx, &outNbElement, &outKMA[0]);
    TA_RSI (startIdx, endIdx, &price_close[0], DONEM_RSI, &outBegIdx, &outNbElement, &outRSI[0]);
    TA_AROONOSC(startIdx, endIdx, &price_high[0], &price_low[0], DONEM_ARN, &outBegIdx, &outNbElement, &outARN[0]);

    TA_MACD(startIdx, endIdx, &price_close[0], optInFastPeriod, optInSlowPeriod, optInSignalPeriod, &outBegIdx, &outNbElement, &outMACD[0],  &outMACDSignal[0], &outMACDHist[0]);
    TA_STOCHRSI(startIdx,endIdx, &price_close[0],optInTimePeriod, optInFastK_Period, optInFastD_Period,TA_MAType_SMA, &outBegIdx, &outNbElement,&outFastK[0], &outFastD[0]);
    TA_BBANDS(startIdx, endIdx, &price_close[0], DONEM_BBA, optInNbDevUp, optInNbDevDn, TA_MAType_SMA, &outBegIdx, &outNbElement, &outUpperBand[0], &outMiddleBand[0], &outLowerBand[0]);

    for (int i = 0,ind= outBegIdx; i < outNbElement -1; i++, ind++) {
        std::cout   << price_close[ind] << "\t"
                    << outSMA[i] << "\t" 
                    << outEMA[i] <<  "\t" 
                    << outKMA[i] << "\t"
                    << outRSI[i] << "\t"
                    << outARN[i] << "\t"
                    << outMACD[i] << "\t"
                    << outMACDSignal[i] << "\t"
                    << outMACDHist[i] << "\t"
                    << outFastK[i] << "\t"
                    << outFastD[i] << "\t"
                    << outUpperBand[i] << "\t"
                    << outMiddleBand[i] << "\t"
                    << outLowerBand[i]<<std::endl;
    }
    std::cout << outBegIdx << "\t" << outNbElement << "\t"<<BOY<<std::endl;
    TA_Shutdown();
    system("pause");
}