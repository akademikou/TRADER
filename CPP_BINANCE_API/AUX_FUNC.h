#pragma once
/**************************************************************************************************************/
void  YUVARLA(double& _in, const int _decimal_places) {
	const double multiplier = std::pow(10.0, _decimal_places);
	_in = std::round(_in * multiplier) / multiplier;
}

/**************************************************************************************************************/

void SetCursorPosition(const int x, const int y) {
	COORD position;
	position.X = 2 * (SHORT)x;
	position.Y = (SHORT)y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);//This is the way, just remember the format
}
/**************************************************************************************************************/
std::string Stamp2Time(long long timestamp) {
//	int ms = timestamp % 1000;//take milliseconds
	time_t tick = (time_t)(timestamp / 1000);//conversion time
	struct tm tm;
	char s[40];
	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
	std::string str(s);
//	str = str + " " + std::to_string(ms);
	str = str + " ";
	return str;
}
/**************************************************************************************************************
void print_depthCache() {
	std::map < std::string, std::map <double, double> >::iterator it_i;
	for (it_i = depthCache.begin(); it_i != depthCache.end(); it_i++) {
		std::string bid_or_ask = (*it_i).first;
		std::cout << bid_or_ask << '\n';
		std::cout << "Price             Qty" << '\n';
		std::map <double, double>::reverse_iterator it_j;
		for (it_j = depthCache[bid_or_ask].rbegin(); it_j != depthCache[bid_or_ask].rend(); it_j++) {
			double price = (*it_j).first;
			double qty = (*it_j).second;
			printf("%.08f          %.08f\n", price, qty);
		}
	}
}
/**************************************************************************************************************
void print_klinesCache() {
	std::map < long, std::map<std::string, double> >::iterator it_i;
	std::cout << "==================================" << '\n';
	for (it_i = klinesCache.begin(); it_i != klinesCache.end(); it_i++) {
		long start_of_candle = (*it_i).first;
		std::map <std::string, double> candle_obj = (*it_i).second;
		std::cout << "s:" << start_of_candle << ",";
		std::cout << "o:" << candle_obj["o"] << ",";
		std::cout << "h:" << candle_obj["h"] << ",";
		std::cout << "l:" << candle_obj["l"] << ",";
		std::cout << "c:" << candle_obj["c"] << ",";
		std::cout << "v:" << candle_obj["v"];
		std::cout << " " << '\n';
	}
}
/**************************************************************************************************************
void print_aggTradeCache() {
	std::map < long, std::map<std::string, double> >::iterator it_i;
	std::cout << "==================================" << '\n';
	for (it_i = aggTradeCache.begin(); it_i != aggTradeCache.end(); it_i++) {
		long timestamp = (*it_i).first;
		std::map <std::string, double> aggtrade_obj = (*it_i).second;
		std::cout << "T:" << timestamp << ", ";
		printf("p: %.08f, ", aggtrade_obj["p"]);
		printf("q: %.08f ", aggtrade_obj["q"]);
		std::cout << " " << '\n';
	}
}
/**************************************************************************************************************
void print_userBalance() {
	std::map < std::string, std::map<std::string, double> >::iterator it_i;
	std::cout << "==================================" << '\n';
	for (it_i = userBalance.begin(); it_i != userBalance.end(); it_i++) {
		std::string symbol = (*it_i).first;
		std::map <std::string, double> balance = (*it_i).second;
		std::cout << "Symbol :" << symbol << ", ";
		printf("Free   : %.08f, ", balance["f"]);
		printf("Locked : %.08f ", balance["l"]);
		std::cout << " " << '\n';
	}
}
/**************************************************************************************************************
int ws_depth_onData(Json::Value& json_result) {
	int new_updateId = json_result["u"].asInt();
	if (new_updateId > lastUpdateId) {
		for (unsigned int i = 0; i < json_result["b"].size(); i++) {
			double price = atof(json_result["b"][i][0].asString().c_str());
			double qty = atof(json_result["b"][i][1].asString().c_str());
			if (qty == 0.0) { depthCache["bids"].erase(price); }
			else { depthCache["bids"][price] = qty; }
		}
		for (unsigned int i = 0; i < json_result["a"].size(); i++) {
			double price = atof(json_result["a"][i][0].asString().c_str());
			double qty = atof(json_result["a"][i][1].asString().c_str());
			if (qty == 0.0) { depthCache["asks"].erase(price); }
			else { depthCache["asks"][price] = qty; }
		}
		lastUpdateId = new_updateId;
	}
	print_depthCache();
	return 0;
}
/**************************************************************************************************************
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
/**************************************************************************************************************
int ws_aggTrade_OnData(Json::Value& json_result) {
	long timestamp = (long)json_result["T"].asInt64();
	aggTradeCache[timestamp]["p"] = atof(json_result["p"].asString().c_str());
	aggTradeCache[timestamp]["q"] = atof(json_result["q"].asString().c_str());
	print_aggTradeCache();
	return 0;
}
/**************************************************************************************************************
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
/**************************************************************************************************************/
