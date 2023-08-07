#include <string>
#include <iostream>
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "json.h"
extern BinaCPP MY_CONNECTION;
extern Json::Value MY_result;
extern long MY_recvWindow;
#include "CLASS_TRADER.h"
#define RESET   "\033[0m"
#define BOLDYELLOW  "\033[1;33m"
/**************************************************************************************************************/
void  YUVARLA(double& _in, const int _decimal_places) {
	const double multiplier = std::pow(10.0, _decimal_places);
	_in = std::round(_in * multiplier) / multiplier;
}
/**************************************************************************************************************/
TRADER::TRADER(string _name,const int _id) {
	YON = 0;
	EMIR = 0;

	SMA_25_LAST = 0;
	SMA_99_LAST = 0;
	EMA_LAST = 0;
	KMA_LAST = 0;

	SMA_LAST = 0;
	TRG_LAST = 0;
	BOL_LAST = 0;
	ONC_LAST = 0;

	RSI_LAST = 0;
	ARN_LAST = 0;
	MAC_LAST = 0;
	STC_LAST = 0;

	LAST_BUY=0;
	LAST_SEL=0;
	name = _name;
	id = _id;
	HISTORY.clear();
	stTIME=' ';
	Clear_Order();
	HISTORY.clear();
}
/**************************************************************************************************************/
void TRADER::PRINT(){
	std::string stYAZ;
	unsigned int BOY = (unsigned int)HISTORY.size();	
	if (BOY > 10) { 
		for (unsigned int i = 0; i < 10; i++) {HISTORY[i] = HISTORY[BOY - 10 +i];}
		BOY = 10; 
		HISTORY.resize(10);
	}
	for (unsigned int i = 0; i <BOY ; i++) {
		stYAZ = HISTORY[BOY - 1 - i];
		while (stYAZ.length() < 160) {stYAZ += "_";};
		std::cout <<stYAZ << '\n';
	}
	std::cout << BOLDYELLOW << "____________________________________________________________________________________________" << RESET << '\n';
}
/**************************************************************************************************************/
void TRADER::Clear_Order() {
	EMIR = 0;

	SMA_25 = 0;
	SMA_99 = 0;
	EMA = 0;
	KMA = 0;

	SMA = 0;
	TRG = 0;
	BOL = 0;
	ONC = 0;

	RSI = 0;
	ARN = 0;
	MAC = 0;
	STC = 0;
}
/**************************************************************************************************************/
void TRADER::Check_Consistency() {	// TUTARSIZ EMIRLER	

	if (SMA_25_LAST * SMA_25 > 0) { SMA_25 = 0; }
	if (SMA_99_LAST * SMA_99 > 0) { SMA_99 = 0; }
	if (EMA_LAST * EMA > 0) { EMA = 0; }
	if (KMA_LAST * KMA > 0) { KMA = 0; }

	if (SMA_LAST * SMA > 0) { SMA = 0; }
	if (TRG_LAST * TRG > 0) { TRG = 0; }
	if (BOL_LAST * BOL > 0) { BOL = 0; }
	if (ONC_LAST * ONC > 0) { ONC = 0; }

	if (RSI_LAST * RSI > 0) { RSI = 0; }
	if (ARN_LAST * ARN > 0) { ARN = 0; }
	if (MAC_LAST * MAC > 0) { MAC = 0; }
	if (STC_LAST * STC > 0) { STC = 0; }

	int CNT_SEL = 0;
	int CNT_BUY = 0;

	if (SMA_25 == 1) { CNT_BUY++; }else if (SMA_25 == -1) { CNT_SEL--; }
	if (SMA_99 == 1) { CNT_BUY++; }else if (SMA_99 == -1) { CNT_SEL--; }
	if (EMA == 1) { CNT_BUY++; }else if (EMA == -1) { CNT_SEL--; }
	if (KMA == 1) { CNT_BUY++; }else if (KMA == -1) { CNT_SEL--; }
	
	if (SMA == 1) { CNT_BUY++; }else if (SMA == -1) { CNT_SEL--; }
	if (TRG == 1) { CNT_BUY++; }else if (TRG == -1) { CNT_SEL--; }
	if (BOL == 1) { CNT_BUY++; }else if (BOL == -1) { CNT_SEL--; }
	if (ONC == 1) { CNT_BUY++; }else if (ONC == -1) { CNT_SEL--; }

	if (RSI == 1) { CNT_BUY++; }else if (RSI == -1) { CNT_SEL--; }
	if (ARN == 1) { CNT_BUY++; }else if (ARN == -1) { CNT_SEL--; }
	if (MAC == 1) { CNT_BUY++; }else if (MAC == -1) { CNT_SEL--; }
	if (STC == 1) { CNT_BUY++; }else if (STC == -1) { CNT_SEL--; }

	if (CNT_BUY * CNT_SEL < 0) { Clear_Order(); return; }	// AYNI ANDA ALIS VE SATIS EMRI
	if (CNT_BUY > 0) { EMIR = 1; }else if (CNT_SEL < 0) { EMIR = -1; }else { EMIR = 0; }
	if (YON > 0 && EMIR < 0) { Clear_Order(); return; }	// ARTMAYA DEVAM EDIYOR SATACAKSAN BEKLE
	if (YON < 0 && EMIR > 0) { Clear_Order(); return; }	// AZALMAYA DEVAM EDIYOR ALACAKSAN BEKLE
}
/**************************************************************************************************************/
void TRADER::Record_Order(const double& PRICE) {
	if (EMIR > 0) { LAST_BUY = PRICE; }
	if (EMIR < 0) { LAST_SEL = PRICE; }

	if (SMA_25 != 0) { SMA_25_LAST = SMA_25;}
	if (SMA_99 != 0) { SMA_99_LAST = SMA_99;}
	if (EMA != 0) { EMA_LAST = EMA;}
	if (KMA != 0) { KMA_LAST = KMA;}

	if (SMA != 0) { SMA_LAST = SMA;}
	if (TRG != 0) { TRG_LAST = TRG;}
	if (BOL != 0) { BOL_LAST = BOL;}
	if (ONC != 0) { ONC_LAST = ONC;}

	if (RSI != 0) { RSI_LAST = RSI;}
	if (ARN != 0) { ARN_LAST = ARN;}
	if (MAC != 0) { MAC_LAST = MAC;}
	if (STC != 0) { STC_LAST = STC;}
	Clear_Order();
}
/**************************************************************************************************************/
bool TRADER::TRADE(const std::string _name, const double _wallet_CURR, const double _wallet_USDT) {
//	std::cout << "101" << '\n';//________________________________________________________________________________________
	Check_Consistency();
	if (EMIR == 0) {return false;}
//	std::cout << "102" << '\n';//________________________________________________________________________________________
	double PRICE_CURR = MY_CONNECTION.get_price(_name.c_str());
	double ORAN = 0;
	std::string side;
	std::string INFO = _name;
	if (EMIR > 0) { side = "BUY";  INFO += " -> AL "; }
	if (EMIR < 0) { side = "SELL"; INFO += " -> SAT "; }

	if (SMA_25 != 0) { INFO += "(SMA_25) "; ORAN += 1; }
	if (SMA_99 != 0) { INFO += "(SMA_99) "; ORAN += 1; }
	if (EMA != 0) { INFO += "(EMA) "; ORAN += 1; }
	if (KMA != 0) { INFO += "(KMA) "; ORAN += 1; }
	
	if (SMA != 0) { INFO += "(SMA) "; ORAN += 1; }
	if (TRG != 0) { INFO += "(TRG) "; ORAN += 1; }
	if (BOL != 0) { INFO += "(BOL) "; ORAN += 1; }
	if (ONC != 0) { INFO += "(ONC) "; ORAN += 1; }

	if (RSI != 0) { INFO += "(RSI) "; ORAN += 1; }
	if (ARN != 0) { INFO += "(ARN) "; ORAN += 1; }
	if (MAC != 0) { INFO += "(MAC) "; ORAN += 1; }
	if (STC != 0) { INFO += "(STC) "; ORAN += 1; }

	INFO += " " + stTIME + " ";
	/*******************************************************************************/
//	std::cout << "103" << '\n';//________________________________________________________________________________________
	int R = 0;
		 if (_name == "BTCUSDT") { R = 4; }		//					0.0001 BTC
	else if (_name == "LINKUSDT") { R = 2; }	// 0.1		LINK	0.0001 BTC
	else if (_name == "BCHUSDT") { R = 3; }		// 0.001	BCH		0.0001 BTC
	else if (_name == "BNBUSDT") { R = 3; }		// 0.01		BNB		0.0001 BTC
	else if (_name == "XRPUSDT") { R = 1; }		// 1		XRP
	else if (_name == "ETHUSDT") { R = 3; }		// 0.001	ETH		 0.0001 BTC
	else if (_name == "BTTUSDT") { R = 0; }		// 40000	BTT

	double BAS_USDT = 40.0;
	double LOW_USDT = 250.0;
	double BAS_COIN = BAS_USDT / PRICE_CURR;
	double LOW_COIN = LOW_USDT / PRICE_CURR;

	double MIKTAR_USDT = BAS_USDT * ORAN;
	double MIKTAR_COIN = MIKTAR_USDT / PRICE_CURR;

	bool YETERLI = true;
//	std::cout << "104" << '\n';//________________________________________________________________________________________
	if (EMIR > 0) { // AL
		if ((_wallet_USDT - MIKTAR_USDT) <= LOW_USDT) { MIKTAR_USDT = (_wallet_USDT - LOW_USDT); }
		if (MIKTAR_USDT < (BAS_USDT * 0.9)) { YETERLI = false; }
		MIKTAR_COIN = MIKTAR_USDT / PRICE_CURR;		
	}
	if (EMIR < 0) { // SAT
		if ((_wallet_CURR - MIKTAR_COIN) <= LOW_COIN) { MIKTAR_COIN = (_wallet_CURR - LOW_COIN); }
		if (MIKTAR_COIN < (BAS_COIN * 0.9)) { YETERLI = false; }
	}
	if (YETERLI == false) {
		INFO += "_wallet_CURR "+ std::to_string(_wallet_CURR)+" ";
		INFO += "MIKTAR_COIN " + std::to_string(MIKTAR_COIN)+ " ";

		INFO += "_wallet_USDT " + std::to_string(_wallet_USDT) + " ";
		INFO += "MIKTAR_USDT " + std::to_string(MIKTAR_USDT) + " ";
		INFO += "CUZDAN ISLEM ICIN YETERLI DEGIL ";
		HISTORY.push_back(INFO);
		return false;
	}
	YUVARLA(MIKTAR_COIN, R);
	MIKTAR_USDT = MIKTAR_COIN * PRICE_CURR;
	INFO += " price: "+std::to_string(PRICE_CURR);
	INFO += " quantity(USDT): "+std::to_string(MIKTAR_USDT);
	HISTORY.push_back(INFO);
	
	if(EMIR!=0){
		const char* type = "MARKET";	// "LIMIT"
		const char* timeInForce = "GTC";
		double quantity = MIKTAR_COIN;
		double price = 0;
		const char* newClientOrderId = "";
		double stopPrice = 0;
		double icebergQty = 0;
//		std::cout << "105" << '\n';//________________________________________________________________________________________
		MY_CONNECTION.send_order(_name.c_str(), side.c_str(), type, timeInForce, quantity, price, newClientOrderId, stopPrice, icebergQty, MY_recvWindow, MY_result);
	//	std::cout << MY_result << '\n';
	}
//	std::cout << "106" << '\n';//________________________________________________________________________________________
	Record_Order(PRICE_CURR);
//	std::cout << "107" << '\n';//________________________________________________________________________________________
	return false;
//	return true;
}
/**************************************************************************************************************/