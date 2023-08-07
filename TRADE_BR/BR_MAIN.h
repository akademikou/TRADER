#ifndef PATH
	#if defined(_MSC_VER)    //  Microsoft
		#define PATH "C:/Users/akade/Documents/R_PROJ_LVQ/"
//		#define PATH ""
	#elif defined(__GNUC__)   //  GCC
		#define PATH "/root/R_PROJ_LVQ/"
	#endif
#endif
/***********************************************************************************/
class PAR{
public:
	int K;
	int U;
	int D;
	double A;
public:
	void YAZ(){std::cout<<K<<"\t"<<U<<"\t"<<D<<"\t"<<A<<"\n";}
	PAR(){K=U=D=0; A=0.0;}
	PAR(int _K,int _U,int _D,double _A){
		K=_K;
		U=_U;
		D=_D;
		A=_A;
	}
};

class BR_PAR{
public:
	PAR brPAR;
	int iS;
	int iE;
	double RET;
public:
	BR_PAR(){
		brPAR= PAR(0,0,0,0.0);
		iS   =0;
		iE   =0;
		RET  =0.0;
	}
	BR_PAR(PAR _PAR,int _iS,int _iE,double _RET){
		brPAR= PAR(_PAR.K,_PAR.U,_PAR.D,_PAR.A);
		iS   =_iS;
		iE   =_iE;
		RET  =_RET;
	}
};

int	nDATA;
int	nEN;
double  *RSI	= NULL;
double **DATA	= NULL;

#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif



/***********************************************************************************/
void OKU_DATA(std::string name,std::vector<std::vector<double>>&rawDATA) {

	std::string line;
	double dTmp;
	std::vector<double> vTmp;
	std::string sTmp;

	rawDATA.clear();
	std::fstream DOSYA;

	DOSYA.open(name.c_str(), std::ios::in);
	if(!DOSYA.is_open()){std::cout<<name.c_str()<<" not opened\n";}
	std::getline(DOSYA, line);
	while (std::getline(DOSYA, line)){
		std::istringstream iss(line);
		while(iss>>sTmp){
			if(sTmp!="NA"){dTmp=std::stod(sTmp);}else{dTmp=0;}
			vTmp.push_back(dTmp);
		}
		rawDATA.push_back(vTmp);
		vTmp.clear();
	}
	DOSYA.close();
}
/***********************************************************************************/
void DE_ALLOC() {
	Free_Matrix_1D(RSI);
	Free_Matrix_2D(DATA);
//	Free_Matrix_2D(EMA);
}
/***********************************************************************************/
void INIT(std::string F_NAME) {
	std::vector<std::vector<double>>vecTMP_1;
	std::vector<std::vector<double>>vecTMP_2;
	std::string NAME_1="T_RSI.txt";
	std::string NAME_2="T_"+F_NAME+".txt";
	OKU_DATA(PATH+NAME_1,vecTMP_1);
	OKU_DATA(PATH+NAME_2,vecTMP_2);
	/***************************************************************/
	nDATA  = MIN((int)vecTMP_1.size(),(int)vecTMP_2.size());

	std::vector<std::vector<double>>vec_AKTAR_1;
	std::vector<std::vector<double>>vec_AKTAR_2;
	vec_AKTAR_1.clear();
	vec_AKTAR_2.clear();
	for(int i=0;i<(int)vecTMP_1.size();i++){vec_AKTAR_1.push_back(vecTMP_1[i]);}
	for(int i=0;i<(int)vecTMP_2.size();i++){vec_AKTAR_2.push_back(vecTMP_2[i]);}
	/*******************************/
	/*******************************/
	vecTMP_1.clear();
	vecTMP_2.clear();
	for(int i=0;i<nDATA;i++){
		vecTMP_1.push_back(vec_AKTAR_1[i]);
		vecTMP_2.push_back(vec_AKTAR_2[i]);
	}
	nDATA  = MIN((int)vecTMP_1.size(),(int)vecTMP_2.size());
	/***************************************************************/
	nEN		= (int)vecTMP_1[0].size();
	RSI		= Init_Matrix_1D<double>(nDATA);
	DATA	= Init_Matrix_2D<double>(nDATA,nEN);

	for(int i=(nDATA-1);i>=0;i--){
		RSI  [i]=vecTMP_1[i][14];
		for(int j=0;j<nEN;j++){DATA[i][j]=vecTMP_2[i][j];}
	}
	vecTMP_1.clear();
	vecTMP_2.clear();
}
/***********************************************************************************/
void RUN_BR(int nPAR,std::string _NAME1,std::string _NAME2,
	int DEL_1,int DEL_2,BR_PAR *G_PAR){

	INIT(_NAME1);
	int HIGH=nEN;
	int LOW=2;

	const int nTh=8;
	omp_set_num_threads(nTh);
	BR **myBR =new BR*[nTh];
	for(int i=0;i<nTh;i++){	myBR  [i] = new BR(_NAME2,nDATA,DATA,RSI);}

	time_t begin, end,elapsed;
    time(&begin);

    int nTEST=100;
    BR_PAR **RESULT= Init_Matrix_2D<BR_PAR>(nTh+1,nTEST);
    for(int id=0;id<=nTh;id++){for(int n=0;n<100;n++){RESULT[id][n].RET=1E25;}}
	/********************************************************************/
	if(nPAR==1){
		#pragma omp parallel
		{
			MY::uint id = (MY::uint)omp_get_thread_num();
			#pragma omp for
			for(int i1=LOW;i1<HIGH;i1+=DEL_1){
				myBR[id]->CALC_OSC(_NAME2,i1,0,0,0);
				myBR[id]->UYGUNLUK(false);
				int nTEST=(int)myBR[id]->vect_VAL.size();
				for(int n=0;n<nTEST;n++){
					if(RESULT[id][n].RET > myBR[id]->vect_VAL[n]){
						RESULT[id][n].brPAR = PAR(i1,0,0,0);
						RESULT[id][n].iS	=	myBR[id]->vect_iS[n];
						RESULT[id][n].iE	=	myBR[id]->vect_iE[n];
						RESULT[id][n].RET   =	myBR[id]->vect_VAL[n];
					}
				}
			}
		}
	}
	/********************************************************************/
	if(nPAR==2){
		if(_NAME2=="STC"){
			#pragma omp parallel
			{
				MY::uint id = (MY::uint)omp_get_thread_num();
				#pragma omp for
				for(int i1=LOW;i1<HIGH-1;i1+=DEL_1){
					for(int i2=LOW;i2<HIGH;i2+=DEL_2){
						myBR[id]->CALC_OSC(_NAME2,i1,i2,0,0);
						myBR[id]->UYGUNLUK(false);
						int nTEST=(int)myBR[id]->vect_VAL.size();
						for(int n=0;n<nTEST;n++){
							if(RESULT[id][n].RET > myBR[id]->vect_VAL[n]){
								RESULT[id][n].brPAR = PAR(i1,i2,0,0);
								RESULT[id][n].iS	=	myBR[id]->vect_iS[n];
								RESULT[id][n].iE	=	myBR[id]->vect_iE[n];
								RESULT[id][n].RET   =	myBR[id]->vect_VAL[n];
							}
						}
					}
				}
			}
		}else{
			#pragma omp parallel
			{
				MY::uint id = (MY::uint)omp_get_thread_num();
				#pragma omp for
				for(int i1=LOW;i1<HIGH-1;i1+=DEL_1){
					for(int i2=(i1+1);i2<HIGH;i2+=DEL_2){
						myBR[id]->CALC_OSC(_NAME2,i1,i2,0,0);
						myBR[id]->UYGUNLUK(false);
						int nTEST=(int)myBR[id]->vect_VAL.size();
						for(int n=0;n<nTEST;n++){
							if(RESULT[id][n].RET > myBR[id]->vect_VAL[n]){
								RESULT[id][n].brPAR = PAR(i1,i2,0,0);
								RESULT[id][n].iS	=	myBR[id]->vect_iS[n];
								RESULT[id][n].iE	=	myBR[id]->vect_iE[n];
								RESULT[id][n].RET   =	myBR[id]->vect_VAL[n];
							}
						}
					}
				}
			}
		}
	}
	/********************************************************************/
	if(nPAR==4){
		const int del_D=5;
		const double del_A=0.025;
		#pragma omp parallel
		{
			MY::uint id = (MY::uint)omp_get_thread_num();
			#pragma omp for
			for(int i1=LOW;i1<HIGH-1;i1+=DEL_1){
				for(int i2=(i1+1);i2<HIGH;i2+=DEL_2){
					for(int PAR_D=10;PAR_D<200;PAR_D+=del_D){
						for(double PAR_A=0.01;PAR_A<=0.8;PAR_A+= del_A){
							myBR[id]->CALC_OSC(_NAME2,i1,i2,PAR_D,PAR_A);
							myBR[id]->UYGUNLUK(false);
							int nTEST=(int)myBR[id]->vect_VAL.size();
							for(int n=0;n<nTEST;n++){
								if(RESULT[id][n].RET > myBR[id]->vect_VAL[n]){
									RESULT[id][n].brPAR = PAR(i1,i2,PAR_D,PAR_A);
									RESULT[id][n].iS	=	myBR[id]->vect_iS[n];
									RESULT[id][n].iE	=	myBR[id]->vect_iE[n];
									RESULT[id][n].RET   =	myBR[id]->vect_VAL[n];
								}
							}
						}
					}
				}
			}
		}
	}
	/********************************************************************/
	nTEST=(int)myBR[0]->vect_iS.size();
	for(int id=0;id<nTh;id++){
		for(int n=0;n<nTEST;n++){
			if( RESULT[nTh][n].RET > RESULT[id][n].RET){
				RESULT[nTh][n].brPAR = PAR(RESULT[id][n].brPAR);
				RESULT[nTh][n].iS	 =		RESULT[id][n].iS;
				RESULT[nTh][n].iE	 =		RESULT[id][n].iE;
				RESULT[nTh][n].RET   =		RESULT[id][n].RET;
			}
		}
	}
	for(int n=0;n<nTEST;n++){
		G_PAR[n].brPAR = PAR(RESULT[nTh][n].brPAR);
		G_PAR[n].iS	   =	 RESULT[nTh][n].iS;
		G_PAR[n].iE	   =	 RESULT[nTh][n].iE;
		G_PAR[n].RET   =	 RESULT[nTh][n].RET;
	}
	std::fstream DOSYA;
	std::string NAME_1="OPT_PAR.txt";
	DOSYA.open(PATH+NAME_1,std::ios::app);
	for(int n=0;n<nTEST;n++){
//		G_PAR[n].brPAR = PAR(RESULT[nTh][n].brPAR);
//		G_PAR[n].iS	   =	 RESULT[nTh][n].iS;
//		G_PAR[n].iE	   =	 RESULT[nTh][n].iE;
//		G_PAR[n].RET   =	 RESULT[nTh][n].RET;

		myBR[0]->CALC_OSC(_NAME2,G_PAR[n].brPAR.K,G_PAR[n].brPAR.U,G_PAR[n].brPAR.D,G_PAR[n].brPAR.A);
		myBR[0]->UYGUNLUK(false);
		DOSYA<<_NAME2<<"\t"
			<<G_PAR[n].brPAR.K<<"\t"
			<<G_PAR[n].brPAR.U<<"\t"
			<<G_PAR[n].brPAR.D<<"\t"
			<<G_PAR[n].brPAR.A<<"\t"
			<<G_PAR[n].iS<<"\t"
			<<G_PAR[n].iE<<"\t"
			<<G_PAR[n].RET<<"\n";

		std::cout<<_NAME2<<"\t"
					<<G_PAR[n].brPAR.K<<"\t"
					<<G_PAR[n].brPAR.U<<"\t"
					<<G_PAR[n].brPAR.D<<"\t"
					<<G_PAR[n].brPAR.A<<"\t"
					<<G_PAR[n].iS<<"\t"
					<<G_PAR[n].iE<<"\t"
					<<G_PAR[n].RET<<"\n";


		int BAS = ((n) == (0)      ? (G_PAR[n  ].iS) : (G_PAR[n].iE));
		int SON = ((n) < (nTEST-1) ? (G_PAR[n+1].iE) : (nDATA) );
		BAS = MIN(nDATA,BAS);
		SON = MIN(nDATA,SON);
		myBR[0]->GET_DATA("BFr_",_NAME2,BAS,SON);
		std::cout<<BAS<<"\t"<<SON<<"\n";

//		BAS = G_PAR[n].iS;
//		SON = ((n) < (nTEST-1) ?  (G_PAR[n+1].iS):(nDATA) );
//		BAS = MIN(nDATA,BAS);
//		SON = MIN(nDATA,SON);
//		myBR[0]->GET_DATA("BFo_",_NAME2,BAS,SON);
//		std::cout<<BAS<<"\t"<<SON<<"\n";

	}
	DOSYA.close();
	std::cout<<"_______________________________\n";


	time(&end);
    elapsed = end - begin;
	std::cout<<_NAME2<<" OPTIMIZASYONU TAMAMLANDI: ("
			<<elapsed<<" sec.)\n***************************\n";

	delete []myBR;
	DE_ALLOC();
}
/***********************************************************************************/
