
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
//#include <time.h>

#include "binacpp.h"
#include "binacpp_websocket.h"
#include "json.h"
//#define API_KEY 		"api key"
//#define SECRET_KEY	"user key"

// https://testnet.binance.vision/api

#define API_KEY 		"as3VwvMInSepAhr5lghESNDRCWiH6UTIvUmbToY6jn4jfbj1WdfrnbTK4G0SdYUC"
#define SECRET_KEY		"IEE2VqIYkiqzSb5NPgWCCbYQGyAJnDDEX3nNmEHA0e8gZJl6EOPKjai5Jv1WO2vb"

// Some code to make terminal to have colors
#define KGRN "\033[0;32;32m"
#define KCYN "\033[0;36m"
#define KRED "\033[0;32;31m"
#define KYEL "\033[1;33m"
#define KBLU "\033[0;32;34m"
#define KCYN_L "\033[1;36m"
#define KBRN "\033[0;33m"
#define RESET "\033[0m"

std::map < std::string, std::map <double, double> >  depthCache;
std::map < long, std::map <std::string, double> >  klinesCache;
std::map < long, std::map <std::string, double> >  aggTradeCache;
std::map < long, std::map <std::string, double> >  userTradeCache;
std::map < std::string, std::map <std::string, double> >  userBalance;


int lastUpdateId;
//------------------------------
void print_depthCache() {
	std::map < std::string, std::map <double, double> >::iterator it_i;

	for (it_i = depthCache.begin(); it_i != depthCache.end(); it_i++) {

		std::string bid_or_ask = (*it_i).first;
		std::cout << bid_or_ask << std::endl;
		std::cout << "Price             Qty" << std::endl;

		std::map <double, double>::reverse_iterator it_j;

		for (it_j = depthCache[bid_or_ask].rbegin(); it_j != depthCache[bid_or_ask].rend(); it_j++) {

			double price = (*it_j).first;
			double qty = (*it_j).second;
			printf("%.08f          %.08f\n", price, qty);
		}
	}
}
//------------------
void print_klinesCache() {
	std::map < long, std::map<std::string, double> >::iterator it_i;
	std::cout << "==================================" << std::endl;
	for (it_i = klinesCache.begin(); it_i != klinesCache.end(); it_i++) {
		long start_of_candle = (*it_i).first;
		std::map <std::string, double> candle_obj = (*it_i).second;
		std::cout << "s:" << start_of_candle << ",";
		std::cout << "o:" << candle_obj["o"] << ",";
		std::cout << "h:" << candle_obj["h"] << ",";
		std::cout << "l:" << candle_obj["l"] << ",";
		std::cout << "c:" << candle_obj["c"] << ",";
		std::cout << "v:" << candle_obj["v"];
		std::cout << " " << std::endl;
	}
}
//---------------
void print_aggTradeCache() {
	std::map < long, std::map<std::string, double> >::iterator it_i;
	std::cout << "==================================" << std::endl;
	for (it_i = aggTradeCache.begin(); it_i != aggTradeCache.end(); it_i++) {
		long timestamp = (*it_i).first;
		std::map <std::string, double> aggtrade_obj = (*it_i).second;
		std::cout << "T:" << timestamp << ", ";
		printf("p: %.08f, ", aggtrade_obj["p"]);
		printf("q: %.08f ", aggtrade_obj["q"]);
		std::cout << " " << std::endl;
	}
}
//---------------
void print_userBalance() {
	std::map < std::string, std::map<std::string, double> >::iterator it_i;
	std::cout << "==================================" << std::endl;
	for (it_i = userBalance.begin(); it_i != userBalance.end(); it_i++) {
		std::string symbol = (*it_i).first;
		std::map <std::string, double> balance = (*it_i).second;
		std::cout << "Symbol :" << symbol << ", ";
		printf("Free   : %.08f, ", balance["f"]);
		printf("Locked : %.08f ", balance["l"]);
		std::cout << " " << std::endl;
	}
}
//-------------
int ws_depth_onData(Json::Value& json_result) {
	int new_updateId = json_result["u"].asInt();
	if (new_updateId > lastUpdateId) {
		for (unsigned int i = 0; i < json_result["b"].size(); i++) {
			double price = atof(json_result["b"][i][0].asString().c_str());
			double qty = atof(json_result["b"][i][1].asString().c_str());
			if (qty == 0.0) {
				depthCache["bids"].erase(price);
			}
			else {
				depthCache["bids"][price] = qty;
			}
		}
		for (unsigned int i = 0; i < json_result["a"].size(); i++) {
			double price = atof(json_result["a"][i][0].asString().c_str());
			double qty = atof(json_result["a"][i][1].asString().c_str());
			if (qty == 0.0) {
				depthCache["asks"].erase(price);
			}
			else {
				depthCache["asks"][price] = qty;
			}
		}
		lastUpdateId = new_updateId;
	}
	print_depthCache();
	return 0;
}
//-------------
int ws_klines_onData(Json::Value& json_result) {
	long  start_of_candle = (long)json_result["k"]["t"].asInt64();
	klinesCache[start_of_candle]["o"] = atof(json_result["k"]["o"].asString().c_str());
	klinesCache[start_of_candle]["h"] = atof(json_result["k"]["h"].asString().c_str());
	klinesCache[start_of_candle]["l"] = atof(json_result["k"]["l"].asString().c_str());
	klinesCache[start_of_candle]["c"] = atof(json_result["k"]["c"].asString().c_str());
	klinesCache[start_of_candle]["v"] = atof(json_result["k"]["v"].asString().c_str());
	print_klinesCache();
	return 0;
}
//-----------
int ws_aggTrade_OnData(Json::Value& json_result) {
	long timestamp = (long)json_result["T"].asInt64();
	aggTradeCache[timestamp]["p"] = atof(json_result["p"].asString().c_str());
	aggTradeCache[timestamp]["q"] = atof(json_result["q"].asString().c_str());
	print_aggTradeCache();
	return 0;
}
//---------------
int ws_userStream_OnData(Json::Value& json_result) {
	std::string action = json_result["e"].asString();
	if (action == "executionReport") {
		std::string executionType = json_result["x"].asString();
		std::string orderStatus = json_result["X"].asString();
		std::string reason = json_result["r"].asString();
		std::string symbol = json_result["s"].asString();
		std::string side = json_result["S"].asString();
		std::string orderType = json_result["o"].asString();
		std::string orderId = json_result["i"].asString();
		std::string price = json_result["p"].asString();
		std::string qty = json_result["q"].asString();
		if (executionType == "NEW") {
			if (orderStatus == "REJECTED") {
				printf(KRED "Order Failed! Reason: %s\n" RESET, reason.c_str());
			}
			//		printf(KGRN"\n\n%s %s %s %s(%s) %s %s\n\n"RESET, symbol.c_str(), side.c_str(), orderType.c_str(), orderId.c_str(), orderStatus.c_str(), price.c_str(), qty.c_str());
			return 0;
		}
		//	printf(KBLU"\n\n%s %s %s %s %s\n\n"RESET, symbol.c_str(), side.c_str(), executionType.c_str(), orderType.c_str(), orderId.c_str());
	}
	else if (action == "outboundAccountInfo") {
		// Update user balance
		for (unsigned int i = 0; i < json_result["B"].size(); i++) {
			std::string symbol = json_result["B"][i]["a"].asString();
			userBalance[symbol]["f"] = atof(json_result["B"][i]["f"].asString().c_str());
			userBalance[symbol]["l"] = atof(json_result["B"][i]["f"].asString().c_str());
		}
		print_userBalance();
	}
	return 0;
}
//---------------------------







#define FROM    "akademikou@gmail.com"
#define TO		"akademikou@gmail.com"
#define CC		"akademikou@gmail.com"
static const char* payload_text[] = {
  "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n",
  "To: " TO "\r\n",
  "From: " FROM "(GONDERICI_ADI)\r\n",
  "Cc: " CC "(Another example User)\r\n",
  "Subject: CPP BOT ISLEM\r\n",
  "\r\n", /* empty line to divide headers from body, see RFC5322 */
  "The body of the message starts here.\r\n",
  "\r\n",
  "It could be a lot of lines, could be MIME encoded, whatever.\r\n",
  "Check RFC5322.\r\n",
  NULL
};

const char* POSTA[8];
struct upload_status {
	int lines_read;
};

static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp){
	struct upload_status* upload_ctx = (struct upload_status*)userp;
	const char* data;
	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}
//	data = payload_text[upload_ctx->lines_read];
	data = POSTA[upload_ctx->lines_read];
	if (data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read++;
		return len;
	}
	return 0;
}
int SEND_EMAIL() {
	POSTA[0] = "Date: Mon, 29 Nov 2000 21:54:29 +1100\r\n";
	POSTA[1] = "To: " TO "\r\n";
	POSTA[2] = "From: " FROM "(GONDERICI_ADI)\r\n";
	POSTA[3] = "Cc: " CC "(KOPYA_ALICI_ADI)\r\n";
	POSTA[4] = "Subject: CPP BOT ISLEM\r\n";
	POSTA[5] = "\r\n"; /* empty line to divide headers from body, see RFC5322 */
	POSTA[6] = "The body of the message starts here.\r\n"
		"\r\n"
		"It could be a lot of lines, could be MIME encoded, whatever.\r\n"
		"Check RFC5322.\r\n";

	POSTA[7] = NULL;
	CURL* curl;
	CURLcode res = CURLE_OK;
	struct curl_slist* recipients = NULL;
	struct upload_status upload_ctx;
	upload_ctx.lines_read = 0;
	curl = curl_easy_init();
	if (curl) {
		/* This is the URL for your mailserver */
		curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
		recipients = curl_slist_append(recipients, TO);
		recipients = curl_slist_append(recipients, CC);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_USERNAME, "akademikou@gmail.com");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "BB11!bb11ab12ab12");
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		curl_slist_free_all(recipients);
		curl_easy_cleanup(curl);
	}
	return (int)res;
}
//--------------------------
int main() {
//	SEND_EMAIL();
//	return 0;

	system("cls");
	std::string api_key = API_KEY;
	std::string secret_key = SECRET_KEY;
	BinaCPP::init(api_key, secret_key);
	/*
		The Json::value object each element can be access like hash map <>,
		or vector <> if it is Json::array
	*/

	//	client.time_offset = 0
	//	res = client.get_server_time()
	//	client.time_offset = res['serverTime'] - int(time.time() * 1000)
	//	timestamp = int(time.time() * 1E3 + client.time_offset)
	//	balance = client.get_asset_balance(asset = 'USDT', timestamp = timestamp, recvWindow = 60000)


	Json::Value result;
	long recvWindow = 60000;

	//	BinaCPP::get_serverTime(result);																		// Example : Get Server Time.
	//	std::cout << result << std::endl;
	//	std::cout << result["serverTime"] << std::endl;
	//	BinaCPP::get_allPrices( result );																		// Example : Get all Prices
	//	std::cout << result << std::endl;
	//	double bnbeth_price = BinaCPP::get_price( "BNBETH");													// Example: Get price of single pair. Eg: BNBETH
	//	std::cout << bnbeth_price << std::endl;
	//	BinaCPP::get_account( recvWindow , result );															// Example: Get Account
	//	std::cout << result << std::endl;


	//	BinaCPP::get_allBookTickers( result );																	// Example : Get all bid/ask prices
	//	std::cout << result << std::endl;	
	//	BinaCPP::get_bookTicker("bnbeth", result );																// Example: Get bid/ask for single pair
	//	std::cout << result << std::endl;
	//	BinaCPP::get_depth( "ETHBTC", 5, result ) ;																// Example: Get Depth of single pair
	//	std::cout << result << std::endl;
	//	BinaCPP::send_order( "BNBETH", "BUY", "LIMIT", "GTC", 20 , 0.00380000, "",0,0, recvWindow, result );	// Example: Placing a LIMIT order
	//	std::cout << result << std::endl;
	//	BinaCPP::send_order( "BNBETH", "BUY", "MARKET", "GTC", 20 , 0,   "",0,0, recvWindow, result );			// Example: Placing a MARKET order
	//	std::cout << result << std::endl;
	//	BinaCPP::send_order( "BNBETH", "BUY", "MARKET", "GTC", 1 , 0,   "",0,20, recvWindow , result );			// Example: Placing an ICEBERG order
	//	std::cout << result << std::endl;
	//	BinaCPP::get_order( "BNBETH", 12345678, "", recvWindow, result );										// Example: Check an order's status
	//	std::cout << result << std::endl;
	//	BinaCPP::cancel_order("BNBETH", 12345678, "","", recvWindow, result);									// Example: Cancel an order
	//	std::cout << result << std::endl;
	//	BinaCPP::get_openOrders( "BNBETH", recvWindow, result ) ;												// Example: Getting list of open orders for specific pair
	//	std::cout << result << std::endl;
	//	BinaCPP::get_allOrders("BNBETH", 0, 0, recvWindow, result);												// Example: Get all account orders; active, canceled, or filled.
	//	std::cout << result << std::endl;
	//	BinaCPP::get_myTrades( "BNBETH", 0,0, recvWindow , result );											// Example : Get all trades history
	//	std::cout << result << std::endl;
	//	BinaCPP::get_24hr( "ETHBTC", result ) ;																	// Example: Getting 24hr ticker price change statistics for a symbol
	//	std::cout << result << std::endl;
	//	BinaCPP::get_klines( "ETHBTC", "1h", 10 , 0, 0, result );												// Example: Get Kline/candlestick data for a symbol
	//	std::cout << result << std::endl;
		/*-------------------------------------------------------------
		/* Websockets Endpoints */
		// Market Depth 
	std::string symbol = "BNBBTC";
	BinaCPP::get_depth(symbol.c_str(), 20, result);

	// Initialize the lastUpdateId
	lastUpdateId = result["lastUpdateId"].asInt();

	for (unsigned int i = 0; i < result["asks"].size(); i++) {
		double price = atof(result["asks"][i][0].asString().c_str());
		double qty = atof(result["asks"][i][1].asString().c_str());
		depthCache["asks"][price] = qty;
	}
	for (unsigned int i = 0; i < result["bids"].size(); i++) {

		double price = atof(result["bids"][i][0].asString().c_str());
		double qty = atof(result["bids"][i][1].asString().c_str());
		depthCache["bids"][price] = qty;
	}
	print_depthCache();

	// Klines/CandleStick
	BinaCPP::get_klines("ETHBTC", "1h", 10, 0, 0, result);
	for (unsigned int i = 0; i < result.size(); i++) {
		long start_of_candle = (long)result[i][0].asInt64();
		klinesCache[start_of_candle]["o"] = atof(result[i][1].asString().c_str());
		klinesCache[start_of_candle]["h"] = atof(result[i][2].asString().c_str());
		klinesCache[start_of_candle]["l"] = atof(result[i][3].asString().c_str());
		klinesCache[start_of_candle]["c"] = atof(result[i][4].asString().c_str());
		klinesCache[start_of_candle]["v"] = atof(result[i][5].asString().c_str());
	}
	print_klinesCache();
	//  AggTrades 
	BinaCPP::get_aggTrades("BNBBTC", 0, 0, 0, 10, result);
	for (unsigned int i = 0; i < result.size(); i++) {
		long timestamp = (long)result[i]["T"].asInt64();
		aggTradeCache[timestamp]["p"] = atof(result[i]["p"].asString().c_str());
		aggTradeCache[timestamp]["q"] = atof(result[i]["q"].asString().c_str());
	}
	print_aggTradeCache();
	// User Balance
	BinaCPP::get_account(recvWindow, result);
	for (unsigned int i = 0; i < result["balances"].size(); i++) {
		std::string symbol_1 = result["balances"][i]["asset"].asString();
		userBalance[symbol_1]["f"] = atof(result["balances"][i]["free"].asString().c_str());
		userBalance[symbol_1]["l"] = atof(result["balances"][i]["locked"].asString().c_str());
	}
	print_userBalance();
	// User data stream	
	BinaCPP::start_userDataStream(result);
	std::cout << result << std::endl;
	std::string ws_path = std::string("/ws/");
	ws_path.append(result["listenKey"].asString());

	BinaCPP_websocket::init();
	BinaCPP_websocket::connect_endpoint(ws_aggTrade_OnData, "/ws/bnbbtc@aggTrade");
	BinaCPP_websocket::connect_endpoint(ws_userStream_OnData, ws_path.c_str());
	BinaCPP_websocket::connect_endpoint(ws_klines_onData, "/ws/bnbbtc@kline_1m");
	BinaCPP_websocket::connect_endpoint(ws_depth_onData, "/ws/bnbbtc@depth");
	BinaCPP_websocket::enter_event_loop();
	return 0;
}