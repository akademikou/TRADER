#ifndef PATH
	#if defined(_MSC_VER)    //  Microsoft
		#define PATH "C:/Users/akade/Documents/R_PROJ_LVQ/"
//		#define PATH ""
	#elif defined(__GNUC__)   //  GCC
		#define PATH "/root/R_PROJ_LVQ/"
	#endif
#endif
/***********************************************************************************/
#pragma once
class BR {
public:
	std::vector<double> vect_VAL;
	std::vector<double> vect_iS;
	std::vector<double> vect_iE;
protected:
	std::string	NAME;
	int			nDATA;
	double**	cDATA;
	double*		cRSI;
	double*		cTMP;
	double*		cCLOSE;
	int*		cOSC;
	double*		GAIN_LOSS;

	const double	TxnFees=0.001;
	public:
		virtual ~BR();
		BR(std::string _NAME,const int _nDATA,
	double ** &_DATA,double * &_RSI);

		void	GET_DATA(std::string _preNAME,std::string _NAME,int BAS, int SON);
		void	CALC_OSC(std::string _NAME,int i1,int i2,int i3,double d2);
		void	UYGUNLUK(bool VERBOSE);
		void	CALC_PCY(const int _PAR_K,const int _PAR_U,const int _PAR_D, const double _alfa);
		void	CALC_EMA(const int _ind,const int _PER,double *MA);
		void	CALC_SMA(const int _ind,const int _PER,double *MA);

	protected:
		inline int	KARAR(int &_oEMIR,const int _i,const int &_son);
		inline int  EXP_KNOW(const double &_net,const int &_oEMIR,const int &_i);

};
/***********************************************************************************/
BR::~BR() {
	Free_Matrix_1D(cRSI);
	Free_Matrix_1D(cTMP);
	Free_Matrix_1D(cCLOSE);
	Free_Matrix_1D(cOSC);
	Free_Matrix_1D(GAIN_LOSS);
}
/***********************************************************************************/
BR::BR(std::string _NAME,const int _nDATA,double ** &_DATA,double * &_RSI){
	NAME	  = _NAME;
	nDATA	  = _nDATA;
	cDATA	  = _DATA;
	cRSI	  = Init_Matrix_1D<double>(nDATA);
	cTMP	  = Init_Matrix_1D<double>(nDATA);
	cCLOSE	  = Init_Matrix_1D<double>(nDATA);
	cOSC	  = Init_Matrix_1D<int>(nDATA);
	GAIN_LOSS = Init_Matrix_1D<double>(nDATA);
	for(int i=0;i<nDATA;i++){
		cRSI  [i]=_RSI[i];
		cTMP  [i]=0;
		cCLOSE[i]=cDATA[i][1];
		cOSC  [i]=0;
		GAIN_LOSS[i]=0;
	}
}
/***********************************************************************************/
inline int  BR::EXP_KNOW(const double &_net,const int &_oEMIR,const int &_i) {
	int nEMIR=0;
	if(_net >  0){nEMIR =  1;}
	else if(_net < -0){nEMIR = -1;}
	if(nEMIR != _oEMIR && _i>=1 ){
		bool buy_txn=	nEMIR > _oEMIR;
		bool sel_txn=	nEMIR < _oEMIR;
		double new_RSI = cRSI[_i  ];
		double old_RSI = cRSI[_i-1];

		if(buy_txn == 1){					// Bu koşullarda SATMA !!!
			if( (new_RSI > 60) ||
				(new_RSI < 20) ||
				(new_RSI < old_RSI )){return _oEMIR;}
		}
		else if(sel_txn == 1){					// Bu koşullarda ALMA !!!
			if( (new_RSI < 40) ||
				(new_RSI > 80) ||
				(new_RSI > old_RSI )){return _oEMIR;}
		}
	}
	return nEMIR;
}
/***********************************************************************************/
void BR::UYGUNLUK(bool VERBOSE) {
	vect_VAL.clear();
	vect_iS.clear();
	vect_iE.clear();
	double VAL=0;
	int nTEST = 1000;
	int nDEL  = 250;
	int iS = 0;
	int iE = iS+nTEST;
	do{
		if(iE>=nDATA){iE=nDATA;}
		/*************************************************************************/
		VAL=0;
		int LAST=0;
		double nTXN = 0;
		double nWIN = 0;
		double nLOS = 0;
		double WALLET[2]={0,0};
		for(int i=0;i<nDATA;i++){GAIN_LOSS[i]=0;}
		int YON=0;
		double ILK_PARA=0;
		double SON_PARA=0;
		double SON_ADET=0;
		double ADET=0;
		double D=0;
		/*************************************************************************/
		for(int i=iS;i<iE;i++){
			GAIN_LOSS[i]=0;
			YON=cOSC[i];
			if(i==(iE-1)){YON=0;}
			if(LAST!=YON){
				D = cCLOSE[i];
				if(ILK_PARA == 0){ILK_PARA=SON_PARA=D;}

				if(YON == 0){ADET = (-SON_ADET);}// paranin tamami kadar al/sat
				else{
					if(YON == 1){ADET = (YON*SON_PARA/D)/(1+TxnFees);}
					else{{ADET = (-2*SON_ADET+(YON*SON_PARA/D))/(1+TxnFees);}}
				}
				double commision = fabs(ADET*D*TxnFees);
				SON_PARA  = SON_PARA-ADET*D-commision;
				SON_ADET  = SON_ADET+ADET;
				WALLET[0]= (WALLET[1]==0) ? ILK_PARA:WALLET[1];
				WALLET[1]=SON_PARA+SON_ADET*D;
				GAIN_LOSS[i]=(WALLET[1]-WALLET[0]);
				nWIN += (GAIN_LOSS[i]>0)? 1:0;
				nLOS += (GAIN_LOSS[i]<0)? 1:0;
				nTXN++;
	//			if(VERBOSE){std::cout<<i<<"\t"<<YON<<"\t"<<i-nILK<<"\t"<<SON_ADET<<"\t"<<SON_PARA<<"\t"<<WALLET[0]<<"\t"<<cCLOSE[i]<<"\n";}
			}
			LAST=YON;
		}
		/*************************************************************************/
		nLOS--;
		nTXN--;
		if(SON_ADET!=0){std::cout<<"SON_ADET-------------"<<SON_ADET<<std::endl;}
		if(SON_ADET>0){SON_PARA  = SON_PARA+SON_ADET * cCLOSE[iE-1]*(1-TxnFees);}
		if(SON_ADET<0){SON_PARA  = SON_PARA-SON_ADET * cCLOSE[iE-1]*(1-TxnFees);}
		double RET=0;
		if(ILK_PARA!=0){RET=(SON_PARA/ILK_PARA)-1;}
		VAL=(-RET);

		double gross_profits=0;
		double gross_loss=0;
		double Big_WIN = 0;
		double Big_LOS = 0;
		for(int i=iS;i<iE;i++){
			if(GAIN_LOSS[i]>0){gross_profits+=GAIN_LOSS[i];}else{gross_loss+=GAIN_LOSS[i];}
			if(Big_WIN<GAIN_LOSS[i]){Big_WIN=GAIN_LOSS[i];}
			if(Big_LOS>GAIN_LOSS[i]){Big_LOS=GAIN_LOSS[i];}
		}

		double Net_Profit		=	gross_profits+gross_loss;
		double Profit_factor	=	(gross_loss==0)		? 0 : gross_profits / gross_loss;
		double Win_Ratio		=	(nTXN==0)			? 0 : nWIN / nTXN;
		double Average_Winner	=	(nWIN==0)			? 0 : gross_profits / nWIN;
		double Average_Looser	=	(nLOS==0)			? 0 : gross_loss / nLOS;
		double Average_Profit	=	(nLOS==0)			? 0 : Net_Profit / nTXN;
		double PL_Ratio			=	(Average_Looser==0)	? 0 : Average_Winner / Average_Looser;

		double SD=0;
		for(int i=iS;i<iE;i++){
			if(GAIN_LOSS[i]!=0){SD+=(GAIN_LOSS[i]-Average_Profit)*(GAIN_LOSS[i]-Average_Profit);}
		}
		SD = (nTXN==0)		? 0 : sqrt((1/nTXN)*SD);
		double Sharp_Ratio = (SD==0) ? 0: Average_Profit / SD;

		if(VERBOSE){
			std::cout<<"__________________\n";
			std::cout<<"Net_Profit"		<<"\t"<<Net_Profit<<"\n";
			std::cout<<"Profit_factor"	<<"\t"<<Profit_factor<<"\n";
			std::cout<<"Win_Ratio"		<<"\t"<<Win_Ratio<<"\n";
			std::cout<<"Average_Winner"	<<"\t"<<Average_Winner<<"\n";
			std::cout<<"Average_Looser"	<<"\t"<<Average_Looser<<"\n";
			std::cout<<"Average_Profit"	<<"\t"<<Average_Profit<<"\n";
			std::cout<<"PL_Ratio"		<<"\t"<<PL_Ratio<<"\n";
			std::cout<<"Sharp_Ratio"	<<"\t"<<Sharp_Ratio<<"\n";
			std::cout<<"Cumm_Ratio"		<<"\t"<<(SON_PARA/ILK_PARA)-1<<"\n";
			std::cout<<"Big_WIN\t"		<<"\t"<<Big_WIN<<"\n";
			std::cout<<"Big_LOS\t"		<<"\t"<<Big_LOS<<"\n";
			std::cout<<"gross_profits"	<<"\t"<<gross_profits<<"\n";
			std::cout<<"gross_loss"		<<"\t"<<gross_loss<<"\n";
			std::cout<<"nWIN\t"		<<"\t"<<nWIN<<"\n";
			std::cout<<"nLOS\t"		<<"\t"<<nLOS<<"\n";
			std::cout<<"nTXN\t"		<<"\t"<<nTXN<<"\n";
			std::cout<<"__________________\n";
		}

	//	if(nTXN< 5)                  {VAL=1E20;}else{VAL=(-RET);};
	//	if(nTXN<10)                  {VAL=1E20;}else{VAL=(-(Net_Profit-Big_WIN+Big_LOS));};
	//	if(nTXN<15 || nLOS<5)        {VAL=1E20;}else{VAL=(-Win_Ratio);};
	//	if(nTXN<15)                  {VAL=1E20;}else{VAL=(-nWIN);};
		if(nTXN<5 || Sharp_Ratio==0){VAL=1E20;}else{VAL=(-Sharp_Ratio);};
	//	if(nTXN<15 || PL_Ratio==0)   {VAL=1E20;}else{VAL= (-PL_Ratio);};
		/*************************************************************************/
		vect_VAL.push_back(VAL);
		vect_iS.push_back(iS);
		vect_iE.push_back(iE);
		iS+=nDEL;
		iE+=nDEL;
	}while(iE<nDATA);
}
/***********************************************************************************/
void BR::GET_DATA(std::string _preNAME,std::string _NAME,int BAS, int SON){
	std::fstream DOSYA;
//	std::string PREF="/root/";
	DOSYA.open(PATH+_preNAME+_NAME+".txt",std::ios::app);
//	std::cout.setf(std::ios::fixed, std::ios::floatfield);
//	std::cout << std::setprecision(6);
	DOSYA.precision(6);
	DOSYA.setf(std::ios::fixed, std::ios::floatfield);
	DOSYA.setf(std::ios::showpoint);

	for(int i=BAS;i<SON;i++){
		DOSYA<<cDATA[i][0]<<"\t"
			 <<cDATA[i][1]<<"\t"
			 <<cTMP[i]<<"\t"
			 <<cOSC[i]<<"\t"
			 <<i<<"\n";
	}
	DOSYA.close();
}
/***********************************************************************************/
void BR::CALC_OSC(std::string _NAME,int i1,int i2,int i3,double d2){
	if(_NAME=="PCY"){CALC_PCY(i1,i2,i3,d2);}
	else if(_NAME=="STC" ){
		double *MA	  = Init_Matrix_1D<double>(nDATA);
		CALC_SMA(i1,i2,MA); // CALC_EMA
		for(int i=0;i<nDATA;i++){cTMP[i]= cDATA[i][i1]-MA[i];}
		Free_Matrix_1D(MA);
	}
	else if(_NAME=="EMAC" || _NAME=="SMAC" || _NAME=="RSEC" || _NAME=="RSIC" ){
		for(int i=0;i<nDATA;i++){cTMP[i]= cDATA[i][i1]-cDATA[i][i2];}
	}else{
		for(int i=0;i<nDATA;i++){cTMP[i]= cDATA[i][i1];}
	}

    if(_NAME=="RSIC"){for(int i=0;i<nDATA;i++){cTMP[i]=      100*cTMP[i];}}
    if(_NAME=="STC" ){for(int i=0;i<nDATA;i++){cTMP[i]=      100*cTMP[i];}}
    if(_NAME=="RSI" ){for(int i=0;i<nDATA;i++){cTMP[i]= -100+2*  cTMP[i];}}
    if(_NAME=="WPR" ){for(int i=0;i<nDATA;i++){cTMP[i]=  100-200*cTMP[i];}}
    if(_NAME=="MFI" ){for(int i=0;i<nDATA;i++){cTMP[i]= -100+2*  cTMP[i];}}

    if(_NAME=="STC" ){
    	for(int i=0;i<(i1+i2);i++){cTMP[i]=0;}
    }
    else{
    	for(int i=0;i<nDATA;i++){if(cDATA[i][i1] == 0 || cDATA[i][i2] == 0){cTMP[i]=0;}	}
    }
	/****************************************************************/
	double BUY=0,SEL=0;
		 if(_NAME=="RSI"){SEL = 50; BUY = -50;}
	else if(_NAME=="WPR"){SEL = 80; BUY = -80;}
	else if(_NAME=="MFI"){SEL = 70; BUY = -70;}
	else if(_NAME=="ADX"){SEL =-20; BUY =  20;}
	else{
		BUY= fabs(cTMP[0]);;
//		for(int i=1;i<nDATA;i++){if(BUY<fabs(cTMP[i])){BUY=fabs(cTMP[i]);}}
//		BUY*=0.001;
//		SEL= -BUY;
		BUY=  0.05;
		SEL= -0.05;
	}
	/****************************************************************/
	cOSC[0]=cOSC[1]=0;
	if(_NAME=="PCY"){
		for(int i=1;i<nDATA;i++){
/*			double P_DEL=(cTMP[i]-cTMP[i-1]);
			if(P_DEL>  0.01 && (cTMP[i]< 1 || cTMP[i-1]< 1)){cOSC[i]=  1;}
			else if(P_DEL< -0.01 && (cTMP[i]>99 || cTMP[i-1]>99)){cOSC[i]= -1;}
		    else{cOSC[i]=cOSC[i-1];}
			if(cTMP[i]>99.99){cOSC[i]=  1;}
			if(cTMP[i]<00.01){cOSC[i]= -1;}
			*/
				 if(cTMP[i] >  5){cOSC[i]=  1;}
			else if(cTMP[i] < -5){cOSC[i]= -1;}
			else                 {cOSC[i]=  cOSC[i-1];}
		}
	}
	else if(_NAME=="RSI" || _NAME=="WPR" || _NAME=="MFI" || _NAME=="ADX" ){
		for(int i=2;i<nDATA;i++){
			bool N2P = cTMP[i] >= BUY && (cTMP[i-1] < BUY || cTMP[i-2] < BUY);
			bool P2N = cTMP[i] <= SEL && (cTMP[i-1] > SEL || cTMP[i-2] > SEL);
				 if(N2P){cOSC[i] =  1;}
			else if(P2N){cOSC[i] = -1;}
			else		{cOSC[i] = cOSC[i-1];}
		}
	}else{
		for(int i=2;i<nDATA;i++){
			bool N2P = cTMP[i] >= BUY && (cTMP[i-1] < SEL || cTMP[i-2] < SEL);
			bool P2N = cTMP[i] <= SEL && (cTMP[i-1] > BUY || cTMP[i-2] > BUY);
				 if(N2P){cOSC[i] =  1;}
			else if(P2N){cOSC[i] = -1;}
			else		{cOSC[i] = cOSC[i-1];}
		}
	}
	for(int i=1;i<nDATA;i++){cOSC[i]= EXP_KNOW(cOSC[i],cOSC[i-1],i);}
	cOSC[nDATA-1]=0;
}
/***********************************************************************************/
void BR::CALC_PCY(const int _PAR_K,const int _PAR_U,const int _PAR_D, const double _alfa){
	if(_PAR_U>EMA_LIM){std::cout<<"EMA HESAP HATASI\n"; exit(0);}
	double DEL=0.0;
	double *dif 	  = Init_Matrix_1D<double>(nDATA);
	for(int i=0  ;i<nDATA ;i++){
		if(cDATA[i][_PAR_U]==0 || cDATA[i][_PAR_K]==0){dif[i]=0;}else
		{dif[i]= cDATA[i][_PAR_U] - cDATA[i][_PAR_K];}
	}

	for(int i=1  ;i<nDATA ;i++){
		int N1 = (_PAR_D<i) ? (i-_PAR_D):0;
		double ENB     = dif[N1];
		double ENK     = dif[N1];
		for(int j=N1 ;j<=i ;j++){
			if(ENB<dif[j]){ENB=dif[j];}
			if(ENK>dif[j]){ENK=dif[j];}
		}
		double ALT = (ENB-ENK);
		if(ALT!=0){ DEL = 100.0*(ENB-dif[i])/ALT;}
		cTMP[i] = _alfa*(DEL-cTMP[i-1])+cTMP[i-1];
	}

	for(int i=1  ;i<nDATA ;i++){dif[i]= cTMP[i]-cTMP[i-1];}
	for(int i=1  ;i<nDATA ;i++){cTMP[i] *= dif[i];}
	Free_Matrix_1D(dif);
}
/***********************************************************************************/
void BR::CALC_EMA(const int _ind,const int _PER,double *MA){
	MA[0]=cDATA[0][_ind];
	for(int i=0  ;i<nDATA ;i++){
		double alpha = 1.0/double(_PER);
		if(i< (_PER-1)){MA[i]=0;}
		if(i==(_PER-1)){
			double sum=0;
			for(int k=0;k<_PER;k++){sum+=cDATA[k][_ind];}
			MA[i]=sum/double(_PER);
		}
		if(i>= _PER){MA[i]=(alpha*cDATA[i][_ind]+(1.0-alpha)*MA[i-1]);}
	}
}
/***********************************************************************************/
void BR::CALC_SMA(const int _ind,const int _PER,double *MA){
	for(int i=0;i<_PER ;i++){
		double sum=0;
		for(int k=0;k<=i;k++){sum+=cDATA[i-k][_ind];}
		MA[i]=sum/double(i+1);
	}
	for(int i=_PER;i<nDATA ;i++){
		double sum=0;
		for(int k=0;k<_PER;k++){sum+=cDATA[i-k][_ind];}
		MA[i]=sum/double(_PER);
	}
}
/***********************************************************************************/
