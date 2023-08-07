#pragma warning (disable:4251)
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sys/timeb.h>
#include <sys/types.h>
#include "CLASS_INDICATOR.h"

int main(){
	system("cls");
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout << std::setprecision(3);
	const std::string interval = "1m";
//	const unsigned int numData = 60*24*60;// 60 gün 86400
	const unsigned int numData = 60*24*5;// 2 gün 86400
	INDICATOR* CRYPT_INDICATOR = new INDICATOR("BTCUSDT", interval.c_str(), numData);
	CRYPT_INDICATOR->GET_DATA();
	CRYPT_INDICATOR->CALC();
	CRYPT_INDICATOR->YAZ_DATA();
	system("pause");
	return 0;
}