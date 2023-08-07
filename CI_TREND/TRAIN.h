#pragma once
#include <sstream>
/************************************************************************************/
void YAZ_INFO(const int& _iter, const int& _int_D, const int& _M_lvq,
	const int& _n1_lvq,const int& _n2_lvq, const int& _Pack, const double& _G_err) {
	std::cout.fill(' '); std::cout.width(10); std::cout << std::left << _iter;
	std::cout.fill(' '); std::cout.width(6); std::cout << std::left << "ORG: ";
	std::cout.fill(' '); std::cout.width(4); std::cout << std::left << _int_D;
	std::cout.fill(' '); std::cout.width(6); std::cout << std::left << "LVQ: ";
	std::cout.fill(' '); std::cout.width(4); std::cout << std::left << _M_lvq;
	std::cout.fill(' '); std::cout.width(10); std::cout << std::left << round(1000 * double(_n1_lvq) / double(_Pack)) / 1000;
	std::cout.fill(' '); std::cout.width(10); std::cout << std::left << round(1000 * double(_n2_lvq) / double(_Pack)) / 1000;
	std::cout.fill(' '); std::cout.width(10); std::cout << std::left << _G_err / _Pack << std::endl;
}
/************************************************************************************/
void CALC_FIYAT(const INDICATOR& _CRYPT_INDICATOR,std::vector<PRICE>&_FYT){
	_FYT.clear();
	int BOY=_CRYPT_INDICATOR.numData;
	for (int i = 0; i < BOY; i++) {
			PRICE tmpP=PRICE(_CRYPT_INDICATOR.price_open[i],
						_CRYPT_INDICATOR.price_high[i],
						_CRYPT_INDICATOR.price_low[i],
						_CRYPT_INDICATOR.price_close[i]);
		_FYT.push_back(tmpP);
	}
}
/************************************************************************************/
void CALC_OUT(const INDICATOR& _CRYPT_INDICATOR,
						std::vector<double>& _vOUT,
						const int& _FWD, const double& _Span) {
	int BOY=_CRYPT_INDICATOR.numData;

	_vOUT.clear();
	_vOUT.resize(BOY);
	
	for (int i = 0; i < BOY-_FWD; i++) {
		double Tmp_0=_CRYPT_INDICATOR.price_close[i];
		double ENB = 0,ENK = 0, sum=0, cnt=0;
		for (int j = i+1; j < i+_FWD; j++) {
			double Tmp=_CRYPT_INDICATOR.price_close[j];
			double DEL=(Tmp-Tmp_0);
			if (ENB < DEL) { ENB = DEL; }
			if (ENK > DEL) { ENK = DEL; }
			sum+=Tmp;
			cnt++;
		}		
		ENB=100.0 * ENB / Tmp_0;
		ENK=100.0 * ENK / Tmp_0;
		if (abs(ENB) > abs(ENK)){_vOUT[i] = ENB;}else { _vOUT[i] = ENK; }
	//	_vOUT[i] = ((sum/cnt)-Tmp_0)*100/Tmp_0;

	//	std::cout<<_vOUT[i]<<"\t"<<(sum/cnt)<<"\t"<<Tmp_0<<"\n";
	}
}
/************************************************************************************/
void CALC_IN(const INDICATOR& _CRYPT_INDICATOR, std::vector<std::vector<double>>& _vIN, const int& _BWD) {
	int BOY=_CRYPT_INDICATOR.numData;
	std::vector<double> vTMP;
	_vIN.clear();
	for (int i = _BWD; i < BOY; i++) {
		vTMP.clear();
		for(int j = i; j >= i-_BWD; j--) {
			double Tmp=(_CRYPT_INDICATOR.price_open[j]+_CRYPT_INDICATOR.price_close[j])*0.5;
			double B = (100 / Tmp);
			
	//		vTMP.push_back(_CRYPT_INDICATOR.price_open	[j]);		// 0
	//		vTMP.push_back(_CRYPT_INDICATOR.price_high	[j]);		// 1
	//		vTMP.push_back(_CRYPT_INDICATOR.price_low	[j]);		// 2
	//		vTMP.push_back(_CRYPT_INDICATOR.price_close	[j]);		// 3
	//		vTMP.push_back(_CRYPT_INDICATOR.price_volume[j]);		// 4

	//		vTMP.push_back(50+(_CRYPT_INDICATOR.LoutSMA_25[j] - Tmp) * B);	// 0	// 5
	//		vTMP.push_back(50*(_CRYPT_INDICATOR.LoutSMA_99[j] - Tmp) * B);	// 1	// 6
	//		vTMP.push_back(50+(_CRYPT_INDICATOR.LoutEMA   [j] - Tmp) * B);	// 2	// 7
	//		vTMP.push_back    (_CRYPT_INDICATOR.LoutSTD   [j] );			// 3	// 8

	//		vTMP.push_back((_CRYPT_INDICATOR.LoutUpperBand [j] - Tmp) * B);	// 4	// 9
	//		vTMP.push_back((_CRYPT_INDICATOR.LoutMiddleBand[j] - Tmp) * B);	// 5	// 10
	//		vTMP.push_back((_CRYPT_INDICATOR.LoutLowerBand [j] - Tmp) * B);	// 6	// 11

			vTMP.push_back(_CRYPT_INDICATOR.LoutRSI[j]);					// 7	// 12
	//		vTMP.push_back( _CRYPT_INDICATOR.LoutARN[j]);					// 8	// 13
	//		vTMP.push_back(_CRYPT_INDICATOR.LoutARN_up[j]);					// 9	// 14
	//		vTMP.push_back(_CRYPT_INDICATOR.LoutARN_down[j]);				// 10	// 15
			
	//		vTMP.push_back(_CRYPT_INDICATOR.LoutMACD[j]);					// 11	// 16
	//		vTMP.push_back(_CRYPT_INDICATOR.LoutMACDSignal[j]);				// 12	// 17
	//		vTMP.push_back(_CRYPT_INDICATOR.LoutMACDHist[j]);				// 13	// 18

	//		vTMP.push_back(_CRYPT_INDICATOR.LoutFastK[j]);					// 14	// 19
	//		vTMP.push_back(_CRYPT_INDICATOR.LoutFastD[j]);					// 15	// 20
			vTMP.push_back( _CRYPT_INDICATOR.LoutADX[j]);					// 16	// 21
			vTMP.push_back( _CRYPT_INDICATOR.LoutDIp[j]);					// 17	// 21
			vTMP.push_back( _CRYPT_INDICATOR.LoutDIn[j]);					// 18	// 21

	//		vTMP.push_back((_CRYPT_INDICATOR.LoutFastK[j]-_CRYPT_INDICATOR.LoutFastD[j])*_CRYPT_INDICATOR.LoutRSI[j]);					// 15	// 19
		}
		_vIN.push_back(vTMP);
	}
	for (int i = 0; i < (int)vTMP.size(); i++) {vTMP[i]=0;}
	for (int i = 0; i < _BWD; i++) {_vIN.insert(_vIN.begin(), vTMP);}
}
/************************************************************************************/
void PROCESS_TRAIN_SET(const int& _nCIK, const double& _Span,
			const std::vector<std::vector<double>>& _vIN,
			const std::vector<double>& _vOUT,
			std::vector<std::vector<double>>& _DATA,
			std::vector<std::vector<double>>& _sorted_DATA) {
	int BOY = (int)_vOUT.size();
	_DATA.clear();
	int S=(int)_vIN[0].size();
	std::vector<double>TMP(S+3);
	for (int i = 0; i < BOY; i++) {
		for(int j=0;j<S;j++){TMP[j]=_vIN[i][j];}
		int iTMP=int(nCIK/2)+(int)trunc(_vOUT[i]/_Span);
		TMP[S+0]=MAX(0,MIN(nCIK-1,iTMP));// (round((_vOUT[i]+_Span)*(_nCIK -1)/(2*_Span)));
		TMP[S+1]=(_vOUT[i]);
		TMP[S+2]=(i);
		_DATA.push_back(TMP);
	}
	/**********************************************************/
	_sorted_DATA.clear();
//	int _SON_ADET
//	if (BOY > _SON_ADET) {_DATA.erase(_DATA.begin() + int(BOY - _SON_ADET), _DATA.end());	}
//	BOY = (int)_DATA.size();
	for (int i = 0; i < BOY; i++) {_sorted_DATA.push_back(_DATA[i]);}
//	std::sort(_sorted_DATA.begin(), _sorted_DATA.end() , mycomp);
/*	for (int i = 0; i < BOY; i++) { 
		for (int j = 0; j < nGIR+3; j++) {std::cout << _sorted_DATA[i][j] << "\t";	}
		std::cout << std::endl;
	}
*/
}
/************************************************************************************/
void TRAIN(double **_W, std::vector<std::vector<double>>& _DATA,LVQ* _YSA_LVQ,
	const int &_nGIR, const int& _MID, double & _ERROR) {
	std::random_device	rd1;
	std::mt19937_64		mt1(std::random_device{}());
	std::uniform_int_distribution<int> dist1;
//	mt1.seed(rd1());
	dist1.param(std::uniform_int<int>::param_type{ 0,(int)_DATA.size()-1 });
	int M_lvq = 0, n1_lvq = 0, n2_lvq = 0,int_D = 0;
	int Pack = 5000;
	double err,G_err=0;
	double* G_YSA=new double[_nGIR];
	int cnt=0;
	std::cout << "__________________________________________________________________" << std::endl;
	/*********************************************************************************/
	for (int iter = 0; iter <= 1e4; iter++) {
		int ind = dist1(mt1);
		for (int j = 0; j < _nGIR; j++) { G_YSA[j] = _DATA[ind][j];}
		int_D = (int)_DATA[ind][_nGIR];
		M_lvq = _YSA_LVQ->run(G_YSA, true, int_D);
		if ( int_D == M_lvq) { n1_lvq++; }
		if ((M_lvq < _MID && int_D <= M_lvq) || (M_lvq > _MID && int_D >= M_lvq) || int_D == M_lvq) { n2_lvq++; }
//		if (abs(M_lvq - int_D) <3)  { n_lvq++; }
		G_err += err;
		cnt++;
		if (iter % Pack == 0) {
//			YAZ_INFO(iter, int_D, M_lvq, n1_lvq, n2_lvq, cnt, G_err);
			_ERROR = G_err / Pack;
			n1_lvq = 0;
			n2_lvq = 0;
			G_err = 0.0;
			cnt=0;
		}
	}
	std::cout << "__________________________________________________________________" << std::endl;
	/*********************************************************************************/
	delete[]G_YSA;
}
/************************************************************************************/
void INIT_W(double** _W, std::vector<std::vector<double>>& _DATA,
	int _nGIR, int _nH, int _nPSS) {
	int BOY=(int)_DATA.size();
	for (int i = 0; i < _nH; i++) {
		for (int j = 0; j < _nGIR; j++){
			_W[i][j] = 50+20*(2.0 * double(rand()) / double(RAND_MAX) - 1);
//			_W[i][j] = 100.0*double(rand()) / double(RAND_MAX);
//			_W[i][j] =50;
		}
	}
	return;
	for (int i = 0, ind = 0; i < _nH; ) {
		int n = 1;
		for (int k = 0; k < BOY; k++) {
			if (int(i / _nPSS) == (int)_DATA[k][_nGIR ]) {
				for (int j = 0; j < _nGIR; j++) { _W[i][j] = _DATA[k][j]; }
				n++;
				i++;
				if (i >= _nH || n > _nPSS) { break; }
			}
		}
		i = ind * _nPSS;
		ind++;
	}
#ifdef BINARY
	for (int i = 0; i < _nH; i++) {
		for (int j = 0; j < _nGIR; j++) {
			if (_W[i][j] > 0) { _W[i][j] = 1; }
			else { _W[i][j] = -1; }
		}
	}
#endif
}
/************************************************************************************/