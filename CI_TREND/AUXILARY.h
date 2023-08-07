#pragma once
#include <vector>
#define maxDER 100
#define eMARKET 0
#define eLIMIT 1
#define eSAT -1
#define eAL +1
//	int TYPE;	// 0 MARKET, 1 LIMIT
//	int DIR;	// -1 SELL, +1 BUY
/************************************************************************************/
struct ORDER {
	double done_USD;
	double value_USD;
	double UNIT;
	int TYPE;	// 0 MARKET, 1 LIMIT
	int DIR;	// -1 SELL, +1 BUY
	bool  DONE;
	ORDER() { done_USD=value_USD = UNIT = TYPE =  DIR = 0; DONE = false; }
	ORDER(double _value_USD , double _UNIT, int _TYPE, int _DIR){
		done_USD = 0;
		value_USD = _value_USD;
		UNIT = _UNIT;
		TYPE = _TYPE;
		DIR = _DIR;
		DONE = false;
	}
};
/************************************************************************************/
struct PRICE {
	double OPEN;
	double HIGH;
	double LOW;
	double  CLOSE;
	PRICE() { OPEN = HIGH = LOW = CLOSE = 0; };
	PRICE(const double &_OPEN,
		  const double &_HIGH,
		  const double &_LOW,
		  const double &_CLOSE) {
		OPEN = _OPEN;
		HIGH = _HIGH;
		LOW = _LOW;
		CLOSE = _CLOSE;
	};
	PRICE(const double* in) {
		OPEN = in[0];
		HIGH = in[1];
		LOW = in[2];
		CLOSE = in[3];
	};
	PRICE(const std::vector<double>& in) {
		OPEN = in[0];
		HIGH = in[1];
		LOW = in[2];
		CLOSE = in[3];
	};
};
/************************************************************************************/
double NUM_DIF_B1(const std::vector<double>& V, const int& i) {
	return V[i] != 0 ? 100 * (V[i] - V[i - 1]) / V[i] : maxDER;
}
/************************************************************************************/
double NUM_DIF_B2(const std::vector<double>& V, const int& i) {
#ifdef BINARY
	if ((3 * V[i] - 4 * V[i - 1] + V[i - 2]) > 0) { return 1; }
	else { return -1; }
#else
	return V[i] != 0 ? 100 * 0.5 * (3 * V[i] - 4 * V[i - 1] + V[i - 2]) / V[i] : maxDER;
#endif
}
/************************************************************************************/
double NUM_DIF_B2(const double *V, const int& i) {
#ifdef BINARY
	if ((3 * V[i] - 4 * V[i - 1] + V[i - 2]) > 0) { return 1; }
	else { return -1; }
#else
	return V[i] != 0 ? 100 * 0.5 * (3 * V[i] - 4 * V[i - 1] + V[i - 2]) / V[i] : maxDER;
#endif
}
/************************************************************************************/
bool mycomp(std::vector<double>& A, std::vector<double>& B) { return (A[nINP+1] < B[nINP+1]); }
/************************************************************************************/
void MAF(std::vector<double>& _raw_DATA, std::vector<double>& _filtered_DATA, int nMAF) {
	int BOY = (int)_raw_DATA.size();
	_filtered_DATA.clear();
	for (int i = 0; i < nMAF; i++) {
		double SUM = 0;
		for (int j = 0; j <= i; j++) { SUM += _raw_DATA[j]; }
		_filtered_DATA.push_back(SUM / (i + 1));
	}
	for (int i = nMAF; i < BOY; i++) {
		double SUM = 0;
		for (int j = 0; j < nMAF; j++) { SUM += _raw_DATA[i - j]; }
		_filtered_DATA.push_back(SUM / nMAF);
	}
}
/************************************************************************************/
