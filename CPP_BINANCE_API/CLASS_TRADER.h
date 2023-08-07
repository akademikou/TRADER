#ifndef CLASS_TRADER_H
#define CLASS_TRADER_H
#include <string>
	/*******************************************************************************/
	struct TRADER {
	public:
		int YON;
		int EMIR;

		int SMA_25, SMA_25_LAST;
		int SMA_99, SMA_99_LAST;
		int EMA, EMA_LAST;
		int KMA, KMA_LAST;

		int SMA, SMA_LAST;
		int TRG, TRG_LAST;
		int BOL, BOL_LAST;
		int ONC, ONC_LAST;

		int RSI, RSI_LAST;
		int ARN, ARN_LAST;
		int MAC, MAC_LAST;
		int STC, STC_LAST;

		double LAST_BUY;
		double LAST_SEL;
		std::string stTIME;
		std::string name;
		int id;
		std::vector<std::string> HISTORY;

		TRADER(string _name, const int _id);
		bool TRADE(const std::string _name, const double _wallet_CURR, const double _wallet_USDT);
		void PRINT();
	protected:
		void Clear_Order();
		void Check_Consistency();
		void Record_Order(const double& PRICE);
		
	};
	/*******************************************************************************/
#endif