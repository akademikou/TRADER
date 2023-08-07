#pragma once
#include <iostream>
#include <vector>
/**************************************************************************************************************/
struct ASSET {
public:
	std::string NAME;
	double VAL_F;
	double VAL_L;
	double VAL_USDT;
	double VAL_BTC;
	double PRICE_CURR;
};
/**************************************************************************************************************/
class WALLET {
public:
	std::vector <ASSET> TERM;
	double TOTAL_USDT;
	double TOTAL_BTC;
	double PRICE_BTC;
public:
	virtual ~WALLET() { TERM.clear(); }
	WALLET() { 
		TOTAL_USDT = 0;
		TOTAL_BTC = 0;
		PRICE_BTC = 0;
		TERM.clear(); 
	};
	void REFRESH();
	void PRINT();
	double GET_WALLET_CURR(std::string _name);
	double GET_WALLET_USDT();
};
/**************************************************************************************************************/
double WALLET::GET_WALLET_CURR(std::string _name) {
	for (unsigned int j = 0; j < TERM.size(); j++) {
		if (_name == TERM[j].NAME) { return TERM[j].VAL_F; }
	}
	return 0;
}
/**************************************************************************************************************/
double WALLET::GET_WALLET_USDT() {
	for (unsigned int j = 0; j < TERM.size(); j++) {
		if ("USDT" == TERM[j].NAME) { return TERM[j].VAL_F; }
	}
	return 0;
}
/**************************************************************************************************************/
void WALLET::REFRESH() {
	ASSET aTMP;
	
	MY_CONNECTION.get_account(MY_recvWindow, MY_result);
	PRICE_BTC = MY_CONNECTION.get_price("BTCUSDT");

	double PRICE_CURR;
	double VAL_F;
	double VAL_L;
	std::string  VAL_N;

	TOTAL_USDT = 0;
	TERM.clear();

	for (unsigned int i = 0; i < MY_result["balances"].size(); i++) {
		VAL_F = std::stod(MY_result["balances"][i]["free"].asString());
		VAL_L = std::stod(MY_result["balances"][i]["locked"].asString());
		VAL_N = MY_result["balances"][i]["asset"].asString();
		aTMP.NAME = "";
		if (VAL_F != 0 or VAL_L != 0) {
			std::string stTMP = VAL_N + "USDT";
			if (VAL_N == "USDT") { PRICE_CURR = 1; }
			else { PRICE_CURR = MY_CONNECTION.get_price(stTMP.c_str()); }

			aTMP.NAME = VAL_N;
			aTMP.VAL_F = VAL_F;
			aTMP.VAL_L = VAL_L;
			aTMP.PRICE_CURR = PRICE_CURR;
			aTMP.VAL_USDT = (VAL_F + VAL_L) * PRICE_CURR;
			aTMP.VAL_BTC = (VAL_F + VAL_L) * PRICE_CURR / PRICE_BTC;
			TOTAL_USDT += aTMP.VAL_USDT;

			bool VAR = false;
			for (unsigned int j = 0; j < TERM.size(); j++) {
				if (VAL_N == TERM[j].NAME) {
					TERM[j] = aTMP;
					VAR = true;
					break;
				}
			}
			if (VAR == false) { TERM.push_back(aTMP); }
		}
	}
	TOTAL_BTC = TOTAL_USDT / PRICE_BTC;
}
/**************************************************************************************************************/
void WALLET::PRINT() {
	//KGRN KCYN  KRED KYEL KBLU KCYN_L KBRN
	std::cout << BOLDYELLOW << "____________________________________________________________________________________________" << RESET << '\n';
	std::cout 
		<< YELLOW		<< std::cout.fill(' '); std::cout.width(5); std::cout << std::right << "asset"
		<< BOLDYELLOW	<< std::cout.fill(' '); std::cout.width(9); std::cout << std::right << "price"
		<< BOLDGREEN	<< std::cout.fill(' '); std::cout.width(12); std::cout << std::right << "free"
		<< BOLDRED		<< std::cout.fill(' '); std::cout.width(8); std::cout << std::right << "locked"
		<< BOLDCYAN		<< std::cout.fill(' '); std::cout.width(9); std::cout << std::right << "USDT"
		<< BOLDCYAN		<< std::cout.fill(' '); std::cout.width(7); std::cout << std::right << "BTC"<< RESET << '\n';

	for (unsigned int i = 0; i < TERM.size(); i++) {
		std::cout 
			<< YELLOW		<< std::cout.fill(' '); std::cout.width(5); std::cout << std::right << TERM[i].NAME
			<< BOLDYELLOW	<< std::cout.fill(' '); std::cout.width(9); std::cout << std::right << TERM[i].PRICE_CURR
			<< BOLDGREEN	<< std::cout.fill(' '); std::cout.width(12); std::cout << std::right << TERM[i].VAL_F
			<< BOLDRED		<< std::cout.fill(' '); std::cout.width(8); std::cout << std::right << TERM[i].VAL_L
			<< BOLDCYAN		<< std::cout.fill(' '); std::cout.width(9); std::cout << std::right << TERM[i].VAL_USDT
			<< BOLDCYAN		<< std::cout.fill(' '); std::cout.width(7); std::cout << std::right << TERM[i].VAL_BTC<< RESET << '\n';
	}
	std::cout<< BOLDBLUE;
	std::cout.width(48); std::cout << std::right << TOTAL_USDT;
	std::cout.width(8); std::cout << std::right << TOTAL_BTC<< RESET << '\n';
}
/**************************************************************************************************************/
