#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <ctime>
#include <sys/timeb.h>
#include <sys/types.h>
//#include <time.h>
//#define API_KEY 		"g2ikOhUxGkIe6ajZFqkpPZJ33QTA1QavbNHUiBBVRXT3srtWQbxMkMGYEoHFlBGp"
//#define SECRET_KEY		"D9wOTiMEyxid99vlU2ULGFd5QJzpfiGeR14Biu93yJ4iaLLkenHvcf7FtdOc86kN"





// HMAC_SHA256 key akademikou_BINANCE MC_TEST_SPOT
#define API_KEY 		"LBkHhViix7ik2XDhprH8LSywfx8NpkTbIaBVnKeC3VvhYQlXVZ5daC09XNhjlRBS"
#define SECRET_KEY		"FhRsIHGhFQm6XQIu38hlvMrQ6vWOoPpXhjPepYmvxvIcQlsLxYUsdshvVntioEvW"
std::string api_key = API_KEY;
std::string secret_key = SECRET_KEY;






#include "binacpp.h"
#include "binacpp_websocket.h"
#include "json.h"
BinaCPP MY_CONNECTION;
Json::Value MY_result;
long MY_recvWindow = 60000;
#include "GLOBAL.h"
#include "AUX_FUNC.h"
#include "CLASS_WALLET.h"
#include "CLASS_TRADER__.h"
#include "CLASS_INDICATOR.h"

/**************************************************************************************************************/
int main() {
	system("cls");
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout << std::setprecision(3);
//	std::string api_key = API_KEY;
//	std::string secret_key = SECRET_KEY;
	MY_CONNECTION.init(api_key, secret_key);
	MY_CONNECTION.get_serverTime(MY_result);
	struct _timeb timebuffer;
	_ftime(&timebuffer);
//	unsigned long long int timestamp_B = (unsigned long long int)MY_result["serverTime"].asInt64();	
//	unsigned long long int timestamp_P = unsigned long long int(timebuffer.time) * unsigned long long int(1000) + timebuffer.millitm;
//	unsigned long long int time_offset = timestamp_B - timestamp_P;	
//	std::cout << timestamp_B << '\n';
//	std::cout << timestamp_P << '\n';
// KGRN KCYN  KRED KYEL KBLU KCYN_L KBRN
//	['dateTime', 'open', 'high', 'low', 'close', 'volume', 'closeTime', 'quoteAssetVolume', 'numberOfTrades', 'takerBuyBaseVol', 'takerBuyQuoteVol', 'ignore']

//	MY_CONNECTION.get_allPrices( MY_result );																	std::cout << MY_result << '\n';	// Example : Get all Prices
//	MY_CONNECTION.get_allBookTickers( MY_result );																std::cout << MY_result << '\n';	// Example : Get all bid/ask prices
//	MY_CONNECTION.get_bookTicker("bnbeth", MY_result );															std::cout << MY_result << '\n';	// Example: Get bid/ask for single pair
//	MY_CONNECTION.get_depth		("ETHBTC", 5, MY_result ) ;														std::cout << MY_result << '\n';	// Example: Get Depth of single pair
//	MY_CONNECTION.send_order	("BNBETH","BUY","LIMIT" ,"GTC",20, 0.00380000,"",0, 0,MY_recvWindow,MY_result);	std::cout << MY_result << '\n';	// Example: Placing a LIMIT order
//	MY_CONNECTION.send_order	("BNBETH","BUY","MARKET","GTC",20, 0         ,"",0, 0,MY_recvWindow,MY_result);	std::cout << MY_result << '\n';	// Example: Placing a MARKET order
//	MY_CONNECTION.send_order	("BNBETH","BUY","MARKET","GTC", 1, 0         ,"",0,20,MY_recvWindow,MY_result);	std::cout << MY_result << '\n';	// Example: Placing an ICEBERG order
//	MY_CONNECTION.get_order		("BNBETH", 12345678, "", MY_recvWindow, MY_result );							std::cout << MY_result << '\n';	// Example: Check an order's status
//	MY_CONNECTION.cancel_order	("BNBETH", 12345678, "","", MY_recvWindow, MY_result);							std::cout << MY_result << '\n';	// Example: Cancel an order
//	MY_CONNECTION.get_openOrders("BNBETH", MY_recvWindow, MY_result ) ;											std::cout << MY_result << '\n';	// Example: Getting list of open orders for specific pair
//	MY_CONNECTION.get_allOrders ("BNBETH", 0, 0, MY_recvWindow, MY_result);										std::cout << MY_result << '\n';	// Example: Get all account orders; active, canceled, or filled.
//	MY_CONNECTION.get_myTrades  ("BNBETH", 0,0, MY_recvWindow , MY_result );									std::cout << MY_result << '\n';	// Example : Get all trades history
//	MY_CONNECTION.get_24hr		("ETHBTC", MY_result ) ;														std::cout << MY_result << '\n';	// Example: Getting 24hr ticker price change statistics for a symbol
/*
	"M" -> Was the trade the best price match ?
	"T" -> Timestamp
	"a" -> Aggregate tradeId
	"f" -> First tradeId
	"l" -> Last tradeId
	"m" -> Was the buyer the maker ?
	"p" -> Price
	"q" -> Quantity
*/
//	MY_CONNECTION.get_depth(symbol.c_str(), 20, MY_result);				std::cout << MY_result << '\n';	// 20ser tane asks ve bids
//	MY_CONNECTION.get_aggTrades("BTCUSDT", 0, 0, 0, 10, MY_result);		std::cout << MY_result << '\n';	
//	MY_CONNECTION.start_userDataStream(MY_result);						std::cout << MY_result << '\n';

//	std::string ws_path = std::string("/ws/");
//	ws_path.append(MY_result["listenKey"].asString());
//	BinaCPP_websocket::init();
//	BinaCPP_websocket::connect_endpoint(ws_aggTrade_OnData, "/ws/btcusdt@aggTrade");
//	BinaCPP_websocket::connect_endpoint(ws_userStream_OnData, ws_path.c_str());
//	BinaCPP_websocket::connect_endpoint(ws_klines_onData, "/ws/btcusdt@kline_1m");
//	BinaCPP_websocket::connect_endpoint(ws_depth_onData, "/ws/btcusdt@depth");
//	BinaCPP_websocket::enter_event_loop();
/*
	MY_CONNECTION.get_openOrders("BNBUSDT", MY_recvWindow, MY_result);			std::cout << MY_result << '\n';	// Example: Getting list of open orders for specific pair
	MY_CONNECTION.get_allOrders ("BNBUSDT", 0, 2, MY_recvWindow, MY_result);	std::cout << MY_result << '\n';	// Example: Get all account orders; active, canceled, or filled.
	MY_CONNECTION.get_myTrades  ("BNBUSDT", 2, 0, MY_recvWindow , MY_result );	std::cout << MY_result << '\n';
	system("pause");
	static void get_allOrders(symbol, orderId, limit,         MY_recvWindow, json_result);
	static void get_myTrades (symbol,          limit, fromId, MY_recvWindow, json_result	
*/
	const std::string interval = "1m";
//	const vector<std::string> stASSET = { "BTC","BCH" };
	const std::vector<std::string> stASSET = { "BTC","LINK" };
	const unsigned int nASSET = (int)stASSET.size();
	const unsigned int milliseconds = 1000;
	const unsigned int numData = 110;
	const unsigned int nLAST = 3;	
	/****************************************************************/
	WALLET* myWALLET = new WALLET();
	INDICATOR** CRYPT_INDICATOR = new INDICATOR *[nASSET];
	TRADER** CRYPT_TRADER = new TRADER * [nASSET];
	for (unsigned int i = 0; i < nASSET; i++) {
		CRYPT_INDICATOR[i] = new INDICATOR(stASSET[i]+"USDT", interval.c_str(), nLAST, numData);
		CRYPT_TRADER[i] = new TRADER(stASSET[i] + "USDT",i, myWALLET);
	}

	int* CURR_TRADE = new int[nASSET];
	int* CNT = new int[nASSET];
	for (unsigned int i = 0; i < nASSET; i++) {
		CURR_TRADE[i] = 0;
		CNT[i] = 0;
	}
	std::string stTIME="";
	
	bool OK = false;
	myWALLET->REFRESH();
	
	while (true) {
		/***********************************************************/
		for (unsigned int i = 0; i < nASSET; i++) {
			
			if (CRYPT_INDICATOR[i]->NEW_SECTION) { CNT[i]++; }
			if (CNT[i] >= 10) { CRYPT_TRADER[i]->Clear_Last(); CNT[i] = 0; }
			if (OK == true || CRYPT_INDICATOR[i]->NEW_SECTION) {myWALLET->REFRESH(); }
			CRYPT_INDICATOR[i]->GET_DATA();
			CRYPT_INDICATOR[i]->CALC();
			CRYPT_INDICATOR[i]->STRATEGY_ALL(CRYPT_TRADER[i]);
			OK=CRYPT_TRADER[i]->TRADE(CRYPT_INDICATOR[i]->name, myWALLET->GET_WALLET_CURR(stASSET[i]), myWALLET->GET_WALLET_USDT());
		}
		/***********************************************************/
		SetCursorPosition(0, 0);
		MY_CONNECTION.get_serverTime(MY_result);
		stTIME = Stamp2Time(MY_result["serverTime"].asInt64());
		std::cout << stTIME << '\n';
		myWALLET->PRINT();
		for (unsigned int i = 0; i < nASSET; i++) { CRYPT_INDICATOR[i]->PRINT(CRYPT_TRADER[i]);}		
		for (unsigned int i = 0; i < nASSET; i++) { CRYPT_TRADER[i]->stTIME= stTIME; CRYPT_TRADER[i]->PRINT(); }
//		Sleep(milliseconds);
	}	
	return 0;
}