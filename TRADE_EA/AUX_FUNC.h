#pragma once
/**********************************************************************************************/
void GET_BEST(std::vector<int>&_BEST,const std::vector<std::vector<double>>&_rawDATA){
	const double TxnFees=0.005;
	const double TAX=(1-TxnFees)/(1+TxnFees);
	int nDATA	=(int)_rawDATA.size();
	double *TI_Open	= Init_Matrix_1D<double>(nDATA);
	double *TI_Close= Init_Matrix_1D<double>(nDATA);
	
	for(int i=1;i<nDATA;i++){			// T_STAMP/Op/Hi/Lo/Cl/Vol/ClCl
		TI_Open [i] = _rawDATA[i][1];
		TI_Close[i] = _rawDATA[i][4];
	}
	double	ENTRY	= TI_Open[0];
	int		LAST	= 0;
	int		i		= 0;
	_BEST.resize(size_t(nDATA),0);
	while(i<nDATA){
		if(	(LAST == ( 1) &&  TI_Close[i] > TI_Close[(i-1)]) ||
			(LAST == (-1) &&  TI_Close[i] < TI_Close[(i-1)])){
			_BEST[i] = LAST;
			ENTRY = TI_Close[i];
			i++;
			continue;
		}
		for(int j = i; j<nDATA; j++){
			double R_T = (TI_Close[j]-ENTRY)/ENTRY;
			double R_B = ((1+R_T)*TAX-1);
			double R_S = ((1-R_T)*TAX-1);
			if(	(R_B > 0 || (j==(nDATA-1) && R_T>0) || (LAST==( 1) && R_T>0)) ||
				(R_S > 0 || (j==(nDATA-1) && R_T<0) || (LAST==(-1) && R_T<0))){
				if(R_T!=0){LAST = (R_T>0) ? 1: (-1);}
				_BEST[i] = LAST;
				ENTRY = TI_Close[i];
				i++;
				break;
			}
		}
//		std::cout<<i<<"\t"<<TI_Close[i]<<"\t"<<TI_Close[i-1]<<"\t"<<LAST<<"\n";
	}
	_BEST[nDATA-1] = _BEST[(nDATA-2)];
//	for(int i=1;i<nDATA;i++){_BEST[i-1]=_BEST[i];} // LEAD YAPMAK ICIN
	if (TI_Open	!= NULL) { TI_Open	= Free_Matrix_1D(TI_Open);}
	if (TI_Close!= NULL) { TI_Close	= Free_Matrix_1D(TI_Close);}

}
/**********************************************************************************************/
void YAZ_BEST(std::string name,const std::vector<int>&_BEST) {
	int nDATA	=(int)_BEST.size();
	std::fstream DOSYA;
	DOSYA.open(name.c_str(), std::ios::out);
	if(!DOSYA.is_open()){std::cout<<name.c_str()<<" not opened\n";}
	for(int i=0;i<nDATA;i++){DOSYA<<i<<"\t"<<_BEST[i]<<"\n";}
	DOSYA.close();
}
/**********************************************************************************************/
void WORK_SPACE(std::vector<std::vector<double>>&_DATA,
			std::vector<int>&_BEST_poz,
			std::vector<int>&_BEST_ord) {

	std::string NAME_BEST="BEST_poz.txt";
	std::string NAME_ASSET="ASSET_TI.txt";
	std::vector<std::vector<double>>rawDATA;
	KON::OKU_DATA(PATH+NAME_ASSET,rawDATA);
	KON::GET_DATA(_DATA,rawDATA);
	GET_BEST(_BEST_poz,rawDATA);
	YAZ_BEST(PATH+NAME_BEST,_BEST_poz);
//	std::vector<int>_BEST_ord;
	_BEST_ord.clear();
	_BEST_ord.push_back(0);
	for(int i=1;i<(int)_BEST_poz.size();i++){
		if(_BEST_poz[i]!=_BEST_poz[i-1])
		{_BEST_ord.push_back(_BEST_poz[i]);}else{_BEST_ord.push_back(0);}
	}
	NAME_BEST="BEST_ord.txt";
	YAZ_BEST(PATH+NAME_BEST,_BEST_ord);
}
/**********************************************************************************************/
void LVQ_LEAN(const int &_nGIR,const int &_nCIK,const int &_nPSS,
	const std::vector<int> _BEST_poz,
	const std::vector<int> _BEST_ord,
	std::vector<std::vector<double>> _TI_DATA){

	int			nH		= _nCIK * _nPSS;	
	double**	W		= Init_Matrix_2D<double>(nH, _nGIR);
	for (int i = 0; i < nH; i++){
		for (int j = 0; j < _nGIR; j++){W[i][j] = dist_R_EA(mt_A_EA);}
	}

	const double OK		= 0.01;
	LVQ* YSA_LVQ0 = new LVQ(_nGIR, _nCIK, _nPSS, OK);
	LVQ* YSA_LVQ1 = new LVQ(_nGIR, 2, _nPSS, OK);
	YSA_LVQ0->set_W(W);
	YSA_LVQ0->TRAIN_TEST(_TI_DATA,_BEST_ord,   0,1000,true );
	YSA_LVQ0->TRAIN_TEST(_TI_DATA,_BEST_ord,1000,1100,false);
	YSA_LVQ1->TRAIN_TEST(_TI_DATA,_BEST_poz,   0,1000,true );
	YSA_LVQ1->TRAIN_TEST(_TI_DATA,_BEST_poz,1000,1100,false);
//	for(int i=1;i<(int)BEST.size();i++){BEST[i-1]=BEST[i];}
//	for(int i=(int)BEST.size()-1;i>=0;i--){BEST[i]=BEST[i-1];}
//	YSA_LVQ->TRAIN_TEST(TI_DATA,BEST,0,1000,true);
//	YSA_LVQ->TRAIN_TEST(TI_DATA,BEST,1000,2000,false);	
	std::string	DOSYA_ADI0="LVQW0.txt";
	YSA_LVQ0->get_W(W);
	YSA_LVQ0->yaz_W(PATH+DOSYA_ADI0);
	delete YSA_LVQ0;

	std::string	DOSYA_ADI1="LVQW1.txt";
	YSA_LVQ1->get_W(W);
	YSA_LVQ1->yaz_W(PATH+DOSYA_ADI1);
	delete YSA_LVQ1;

//	DOSYA_ADI="LVQW0.txt";	
//	DOSYA.open(PATH+DOSYA_ADI, std::ios::in);
//	if(!DOSYA.is_open()){std::cout<<PATH+DOSYA_ADI<<" not opened\n";}
//	for (int i = 0; i < nH; i++) {for (int j = 0; j < (nGIR); j++){DOSYA>>W[i][j];}}
//	DOSYA.close();

	KON*	func	= new KON();
	int		nDIM	= func->GET_DIM();
	if(nDIM!=(nH*_nGIR)){
		std::cout<<"SIZE MISMATCH\n";
#if defined(_MSC_VER)
		system("pause");
#elif defined(__GNUC__)
		int sil=0;
		std::cout<<"press 1 to continue\n";
		std::cin>>sil;
#endif
	}
	int BOY=(nH*_nGIR);
	double *IN_tmp;
	IN_tmp = Init_Matrix_1D<double>(BOY);
	for (int i = 0,ind=0; i < nH; i++) {
		for (int j = 0; j < _nGIR; j++){IN_tmp[ind]=W[i][j]; ind++;}
	}
	double SONUC[]={0,0,0};
	/*******************************************************************/
	func->SET_LIM(0,500);
	func->UYGUNLUK(IN_tmp,SONUC[0],true);

	if (IN_tmp	!= NULL) { IN_tmp	= Free_Matrix_1D(IN_tmp);}
	if (W	!= NULL) { W	= Free_Matrix_2D(W);}
	delete func;
#if defined(_MSC_VER)
	//	system("pause");
#elif defined(__GNUC__)
		int sil=0;
		std::cout<<"press 1 to continue\n";
		std::cin>>sil;
#endif
}
/**********************************************************************************************/
void PSO_to_R(const int &_nH,const int &_nGIR,const int _nDATA){
	double			dTmp;
	std::fstream	DOSYA;
	std::string		DOSYA_ADI;
	std::string		line;
	std::vector<double> vTmp;
	std::vector<std::vector<double>>PSO_DATA;
	PSO_DATA.clear();
	DOSYA_ADI="PSO_OPT.txt";
	DOSYA.open(PATH+DOSYA_ADI, std::ios::in);
	if(!DOSYA.is_open()){std::cout<<PATH+DOSYA_ADI<<" not opened\n";}
	while (std::getline(DOSYA, line)){
		std::istringstream iss(line);
		while(iss>>dTmp){vTmp.push_back(dTmp);}
		PSO_DATA.push_back(vTmp);
		vTmp.clear();
	}
	DOSYA.close();
	int nTEST= (int)PSO_DATA.size();
	int nPAR = (int)PSO_DATA[0].size();
	double** PSO_RES= Init_Matrix_2D<double>(nTEST,nPAR);
	for(int i=0;i<(int)PSO_DATA.size();i++){
		for(int j=0;j<(int)PSO_DATA[i].size();j++){PSO_RES[i][j]=PSO_DATA[i][j];}
		PSO_DATA[i].clear();
	}
	PSO_DATA.clear();

	KON*	func	= new KON();
	int		nDIM	= func->GET_DIM();
	if(nDIM!=(_nH*_nGIR)){
		std::cout<<"SIZE MISMATCH\n";
#if defined(_MSC_VER)
		system("pause");
#elif defined(__GNUC__)
		int sil=0;
		std::cout<<"press 1 to continue\n";
		std::cin>>sil;
#endif
	}
	for(int i=0;i<nTEST;i++){
		double	RES = PSO_RES[i][nPAR-3];
		int		ilk	= (int) PSO_RES[i][nPAR-2];
		int		son	= (int) PSO_RES[i][nPAR-1]+100;
		if(ilk>=_nDATA){break;}
		if(son>=_nDATA){son=_nDATA;}
		std::cout<<i<<"\t"<<RES<<"\t"<<ilk<<"\t"<<son<<"\n";
		
		double*IN_tmp = PSO_RES[i];
		DOSYA_ADI="EMIR_RUN_"+std::to_string(i)+".txt";
		func->SET_LIM(ilk,son);
		func->YAZ_EMIR(PATH+DOSYA_ADI, IN_tmp);
		std::cout<<"_________________________________________\n\n";

/*		DOSYA_ADI="LVQW_"+std::to_string(i)+".txt";
		DOSYA.open(PATH+DOSYA_ADI, std::ios::out);
		DOSYA << std::setprecision(6);
		if(!DOSYA.is_open()){std::cout<<PATH+DOSYA_ADI<<" not opened\n";}
		int ind=0;
		for (int i = 0; i < _nH; i++) {
			for (int j = 0; j < (_nGIR-1); j++){DOSYA<<IN_tmp[ind]<<"\t";ind++;}
			DOSYA<<IN_tmp[ind]<<"\n";ind++;
		}
		DOSYA.close();
*/
	}
	DOSYA_ADI="EMIR_TAM.txt";
	func->YAZ_TAM_EMIR(PATH+DOSYA_ADI);
	if (func   != NULL) { delete func;}
}
/**********************************************************************************************/
/**********************************************************************************************/
/**********************************************************************************************/
