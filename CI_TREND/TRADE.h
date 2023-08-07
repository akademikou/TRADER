#pragma once
/************************************************************************************/
void eval_ORDER(const PRICE& _FYT, const double& _commision,
	double& _tUSDT, double& _tCURRENCY, std::vector<ORDER>& _order_LIST) {
//	int TYPE;	// 0 MARKET, 1 LIMIT
//	int DIR;	// -1 SELL, +1 BUY
	int BOY = int(_order_LIST.size());
	for (int i = 0; i < BOY; i++) {
		if (_order_LIST[i].DONE == false) {
			/*******************************************************************/
			if (_order_LIST[i].TYPE == eMARKET) {	// MARKET PRICE
				if (_order_LIST[i].DIR == eAL) {	// BUY
					_order_LIST[i].done_USD = _FYT.CLOSE;
					_tUSDT -= _order_LIST[i].UNIT * (_FYT.CLOSE * (1.0 + _commision));
	//				_tUSDT -= _order_LIST[i].UNIT * (_FYT.HIGH * (1.0 + _commision));
					_tCURRENCY += _order_LIST[i].UNIT;
				}
				if (_order_LIST[i].DIR == eSAT) {// SELL
					_order_LIST[i].done_USD = _FYT.CLOSE;
					_tUSDT += _order_LIST[i].UNIT * (_FYT.CLOSE * (1.0 - _commision));
	//				_tUSDT += _order_LIST[i].UNIT * (_FYT.LOW * (1.0 - _commision));
					_tCURRENCY -= _order_LIST[i].UNIT;
				}
				_order_LIST[i].DONE = true;
			}
			/*******************************************************************/
			if (_order_LIST[i].TYPE == eLIMIT) {	// LIMIT PRICE
				if (_order_LIST[i].DIR == eAL) {	// BUY
					if (_order_LIST[i].value_USD > _FYT.HIGH) {
						_order_LIST[i].done_USD = _order_LIST[i].value_USD;
						_tUSDT -= _order_LIST[i].UNIT * (_order_LIST[i].value_USD * (1.0 + _commision));
						_tCURRENCY += _order_LIST[i].UNIT;
						_order_LIST[i].DONE = true;
					}
				}
				if (_order_LIST[i].DIR == eSAT) {							// SELL
					if (_order_LIST[i].value_USD < _FYT.LOW) {
						_order_LIST[i].done_USD = _order_LIST[i].value_USD;
						_tUSDT += _order_LIST[i].UNIT * (_order_LIST[i].value_USD * (1.0 - _commision));
						_tCURRENCY -= _order_LIST[i].UNIT;
						_order_LIST[i].DONE = true;
					}
				}
			}
		}
	}
}
/************************************************************************************/
int estimate_MARKET(const std::vector<std::vector<double>>& _DATA,
	double* _G_YSA, LVQ* _YSA_LVQ, 
	const int& _nGIR,const int &_curr,double &_err) {
	static double cnt=0;
	static double n_lvq=0;
/*	int _FWD=30;
	for(int i=1;i<10;i++){	// son 10 adet ile eğitim
		for (int j = 0; j < _nGIR; j++) { _G_YSA[j] = _DATA[_curr+_FWD+i][j]; }
		int int_D = (int)_DATA[_curr+i][_nGIR];
		int A=_YSA_LVQ->run(_G_YSA, true, int_D, _err);
	//	std::cout << int_D << "\t" << A << "\t" << _err << std::endl;
	}
*/		
	for (int j = 0; j < _nGIR; j++) { _G_YSA[j] = _DATA[_curr][j]; }
	int _MID=4;
	int int_D = (int)_DATA[_curr][_nGIR];
	int M_lvq=_YSA_LVQ->run(_G_YSA, false, 0);
	if ((M_lvq < _MID && int_D <= M_lvq) || (M_lvq > _MID && int_D >= M_lvq) || int_D == M_lvq) { n_lvq++; }
	cnt++;
//	if((int)cnt%50==0){std::cout<<n_lvq/cnt<<"\t";}
//	if(M_lvq<4 ||M_lvq>6 ){	std::cout<<int_D<<"\t"<<M_lvq<<"\t"<<_err<<"\n";}
	return M_lvq;
}
/************************************************************************************/
void info_ASSET(const PRICE& _FYT,
	const double& _tCURRENCY, const double& _tUSDT) {
//	std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left << _FYT.OPEN;
//	std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left << _FYT.HIGH;
//	std::cout.fill(' '); std::cout.width(12);	std::cout << std::left << _FYT.LOW;
//	std::cout.fill(' '); std::cout.width(12);	std::cout << std::left << _FYT.CLOSE;
	std::cout.fill(' '); std::cout.width(12); 	std::cout << std::left << (_FYT.OPEN+_FYT.CLOSE)*0.5;
	std::cout.fill(' '); std::cout.width(12);	std::cout << _tCURRENCY;
	std::cout.fill(' '); std::cout.width(12);	std::cout << _tUSDT;

	std::cout.fill(' '); std::cout.width(12);
	std::cout << _tUSDT + _tCURRENCY * _FYT.CLOSE;
	std::cout.fill(' '); std::cout.width(12);
	std::cout << _tCURRENCY + _tUSDT / _FYT.CLOSE;
	std::cout << std::endl;
}
/************************************************************************************/

