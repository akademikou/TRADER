namespace MY {typedef unsigned int uint;}
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include "MATRIX.h"

#include "CLASS_KON.h"
#include "CLASS_PSO_DIM.h"

const double	F  = 4.3; //c1+c2+c3;
const double	c1 = 0.9;
const double	c2 = 1.4;			// G_BEST iptal edilmek istenirse c2=sb. c3=0;
const double	c3 = (F-(c1+c2));	// L_BEST iptal edilmek istenirse c2=0   c3=sb;
									// 1>W>(c1+c2+c3)/3-1>0   0.72984
const double K=2/fabs(2-F-sqrt(F*(F-4))); // fi>4 olmalı
/*******************************************************************************/
PSO_DIM::~PSO_DIM() {
	delete []aSNR;
	delete []uSNR;
	delete []araSNR;
	if (P_MEAN	!= NULL) { P_MEAN	= Free_Matrix_2D(P_MEAN);}
	if (G_BEST	!= NULL) { G_BEST	= Free_Matrix_2D(G_BEST);}
	if (VEL		!= NULL) { VEL		= Free_Matrix_2D(VEL);}
	if (Child	!= NULL) { Child	= Free_Matrix_2D(Child);}
	if (dPUB	!= NULL) { dPUB		= Free_Matrix_2D(dPUB);}
	if (P_BEST	!= NULL) { P_BEST	= Free_Matrix_2D(P_BEST);}
	if (L_BEST	!= NULL) { L_BEST	= Free_Matrix_2D(L_BEST);}
	delete []nTH_BSL;
	delete []nTH_BTS;
	delete []nTH_LIM;
	delete []nTH_LIM;
}
/*******************************************************************************/
void PSO_DIM::init_HAFIZA() {
	mt_A_PSO_DIM=new XoshiroCpp::Xoshiro256PlusPlus[nProcessors];
	sum		=new double[nProcessors];
	sq_sum	=new double[nProcessors];	

	aSNR	= new double[nDIM];
	uSNR	= new double[nDIM];
	araSNR	= new double[nDIM];

	P_MEAN	= Init_Matrix_2D<double>(nProcessors,(nDIM+1));
	G_BEST	= Init_Matrix_2D<double>(nProcessors,(nDIM+1));	
	Child	= Init_Matrix_2D<double>(nProcessors,(nDIM+1));	
	dPUB	= Init_Matrix_2D<double>(nPUB, (nDIM+1));
	P_BEST	= Init_Matrix_2D<double>(nPUB, (nDIM+1));
	L_BEST	= Init_Matrix_2D<double>(nPUB, (nDIM+1));
	VEL		= Init_Matrix_2D<double>(nPUB, nDIM);
	for (MY::uint j = 0; j < nDIM; j++) {
		aSNR[j]=0;
		uSNR[j]=0;
		araSNR[j]=0;
	}
	for (MY::uint i = 0; i < nProcessors; i++) {
		for (MY::uint j = 0; j < nDIM; j++) {
			G_BEST[i][j] = 0;
			Child [i][j] =0;
		}
		G_BEST[i][nDIM] = MAX_VAL_PSO_DIM;
		Child [i][nDIM] = MAX_VAL_PSO_DIM;
	}

	#pragma omp parallel for
	for (int i = 0; i < (int)nPUB; i++){
		for (MY::uint j = 0; j < nDIM; j++) {
			VEL   [i][j] =0;
			dPUB  [i][j] =0;
			P_BEST[i][j] =0;
			L_BEST[i][j] =0;			
		}		
		dPUB  [i][nDIM] =MAX_VAL_PSO_DIM;
		P_BEST[i][nDIM] =MAX_VAL_PSO_DIM;
		L_BEST[i][nDIM] =MAX_VAL_PSO_DIM;
	}

	nTH_BSL = new MY::uint[nProcessors];
	nTH_BTS = new MY::uint[nProcessors];
	nTH_LIM = new MY::uint[nPUB];

	MY::uint DEL=(nPUB/nProcessors);
	nTH_BSL[0]=0;
	nTH_BTS[0]=DEL;
	for (MY::uint i = 1; i < nProcessors; i++){
		nTH_BSL[i] = nTH_BSL[i-1]+DEL;
		nTH_BTS[i] = nTH_BTS[i-1]+DEL;
	}
	for (MY::uint i = 0; i < nPUB; i++){
		MY::uint id=MY::uint(i/DEL);
		nTH_LIM[i]= i > (nLOC+nTH_BSL[id]) ? (i-nLOC) : nTH_BSL[id];
	}
	std::cout<<"init_HAFIZA() done \n";
}
/*******************************************************************************/
PSO_DIM::PSO_DIM(KON * _infunc, const MY::uint& _nI) {
	std::cout<<c1<<"\t"<<c2<<"\t"<<c3<<"\t"<<F<<"\t"<<K<<"\n";
	nProcessors = omp_get_max_threads();
	omp_set_num_threads(nProcessors);
	VEL		 = NULL;
	Child	 = NULL;
	dPUB	 = NULL;
	P_BEST	 = NULL;
	L_BEST	 = NULL;

	mut_rate = 0.03;
	cro_rate = 0.20;
	func     = _infunc;
	iterMAX  = _nI;
	nDIM     = func->GET_DIM();
	LDIM	 = nDIM; // 270  /3->90 GRUP /15->18 HUCRE /45->6
	nPUB     = 4*LDIM;
	nPUB	-=nPUB%nProcessors;
	nLOC     = (MY::uint)(nPUB /10 );
	std::cout << "________________________\n\n";
	std::cout << "nDIM\t:" << nDIM << std::endl;
	std::cout << "nPUB\t:" << nPUB << std::endl;
	std::cout << "nLOC\t:" << nLOC << std::endl;
	std::cout << "iterMAX\t:" << iterMAX << std::endl;
	std::cout << "________________________\n\n";
	init_HAFIZA();
	std::uint64_t AA=static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	dist_R_PSO_DIM=std::uniform_real_distribution<double>( 0,1 );
	dist_I_PSO_DIM=std::uniform_int_distribution<int>(0,(int)nPUB-1);
	for(MY::uint i=0;i<nProcessors;i++){mt_A_PSO_DIM[i]=Xoshiro256PlusPlus(AA+i*10+rand()); mt_A_PSO_DIM[i].jump() ;}	
	std::cout<<"PSO constructor done\n";
}
/***********************************************************************************/
void PSO_DIM::SET_BEST(double* EN_IYI){
	for (MY::uint j = 0; j < nDIM; j++){G_BEST[0][j]=EN_IYI[j];}
	func->UYGUNLUK(G_BEST[0],G_BEST[0][nDIM],false);
	std::cout<<G_BEST[0][nDIM]<<"  SET_ BEST -------------------------\n";
}
/***********************************************************************************/
void PSO_DIM::UPDATE_LG_BEST(){
	func->UYGUNLUK(G_BEST[0],G_BEST[0][nDIM],false);
	for (MY::uint i = 1; i < nProcessors; i++) {
		func->UYGUNLUK(G_BEST[i],G_BEST[i][nDIM],false);
		if (G_BEST[0][nDIM] > G_BEST[i][nDIM]) {
			for (MY::uint j = 0; j <= nDIM; j++)
			{G_BEST[0][j] = G_BEST[i][j]; }
		}
	}
	for (MY::uint i = 1; i < nProcessors; i++) {
		for (MY::uint j = 0; j <= nDIM; j++){G_BEST[i][j] = G_BEST[0][j]; }
		func->UYGUNLUK(G_BEST[i],G_BEST[i][nDIM],false);
	}
//	std::cout<<"UPDATE_LG_BEST done\n";
}
/***********************************************************************************/
void PSO_DIM::inject_POP(){
	MY::uint SON=aDIM+LDIM;
	double tmp=1.0/(double)nPUB;
	for (MY::uint i = 1; i < nProcessors; i++) {
		for (MY::uint j = aDIM; j < SON; j++) {P_MEAN[0][j]+=P_MEAN[i][j];}
	}
	for (MY::uint j = aDIM; j < SON; j++) {P_MEAN[0][j]*=tmp;}
	#pragma omp parallel
	{
		MY::uint nRTH = (MY::uint)omp_get_num_threads();
		MY::uint id   = (MY::uint)omp_get_thread_num();
		if(id>=nProcessors){std::cout<<id<<"\t"<<nProcessors<<"\tTHREAD SAYI HATA\n";system("pause");}

		for (MY::uint i = (nPUB-1-(2*id)); i > nPUB*98/100; i-=(2*nRTH)) {
			if(i<0){break;}
			MY::uint i1=i;
			MY::uint i2=i-1;
			for (MY::uint j = aDIM; j < SON; j++) {
				dPUB[i1][j]=G_BEST[0][j]+20*(DoubleFromBits(mt_A_PSO_DIM[id]())-0.5);
					 if (dPUB[i1][j] < aSNR[j]) { dPUB[i1][j] = aSNR[j];}
				else if (dPUB[i1][j] > uSNR[j]) { dPUB[i1][j] = uSNR[j];}

				dPUB[i2][j]=P_MEAN[0][j]+20*(DoubleFromBits(mt_A_PSO_DIM[id]())-0.5);
					 if (dPUB[i2][j] < aSNR[j]) { dPUB[i2][j] = aSNR[j];}
				else if (dPUB[i2][j] > uSNR[j]) { dPUB[i2][j] = uSNR[j];}
			}
			func->UYGUNLUK(dPUB[i1],dPUB[i1][nDIM],false);
			func->UYGUNLUK(dPUB[i2],dPUB[i2][nDIM],false);
			if (P_BEST[i1][nDIM] > dPUB[i1][nDIM]){for (MY::uint j = 0; j <= nDIM; j++) {P_BEST[i1][j] = dPUB[i1][j]; }}
			if (P_BEST[i2][nDIM] > dPUB[i2][nDIM]){for (MY::uint j = 0; j <= nDIM; j++) {P_BEST[i2][j] = dPUB[i2][j]; }}
			if (G_BEST[id][nDIM] > dPUB[i1][nDIM]){for (MY::uint j = 0; j <= nDIM; j++) {G_BEST[id][j] = dPUB[i1][j]; }}
			if (G_BEST[id][nDIM] > dPUB[i2][nDIM]){for (MY::uint j = 0; j <= nDIM; j++) {G_BEST[id][j] = dPUB[i2][j]; }}
		}
	}
}
/***********************************************************************************/
void PSO_DIM::URET() {
	MY::uint SON=aDIM+LDIM;
	#pragma omp parallel
	{

		MY::uint id   = (MY::uint)omp_get_thread_num();
		if(id>=nProcessors){std::cout<<id<<"\t"<<nProcessors<<"\tTHREAD SAYI HATA\n";system("pause");}
		sum[id] =0;
		sq_sum[id] =0;
		for (MY::uint j = aDIM; j < SON; j++) {P_MEAN[id][j]=0;}


		for (MY::uint i = nTH_BSL[id]; i < nTH_BTS[id]; i++) {
			if(i>=nPUB){break;}
			for (MY::uint j = aDIM; j <SON; j++) { L_BEST[i][j] = P_BEST[i][j]; }
			L_BEST[i][nDIM] = P_BEST[i][nDIM];

			for (MY::uint k = nTH_LIM[id]; k < i; k++) {
				if (L_BEST[i][nDIM] > P_BEST[k][nDIM]) {
					for (MY::uint j = aDIM; j <SON; j++) { L_BEST[i][j] = P_BEST[k][j]; }
					L_BEST[i][nDIM] = P_BEST[k][nDIM];
				}
			}

			double U = DoubleFromBits(mt_A_PSO_DIM[id]());
			if (U > cro_rate) {
				double F1=DoubleFromBits(mt_A_PSO_DIM[id]())*c1;
				double F2=DoubleFromBits(mt_A_PSO_DIM[id]())*c2;
				double F3=DoubleFromBits(mt_A_PSO_DIM[id]())*c3;
				double *aVEL	= &VEL[i][0]+aDIM;
				double *adPUB	= &dPUB[i][0]+aDIM;
				double *aP_BEST = &P_BEST[i ][0]+aDIM;
				double *aL_BEST = &L_BEST[i ][0]+aDIM;
				double *aG_BEST = &G_BEST[id][0]+aDIM;

				for (MY::uint j = aDIM; j < SON; j++) {
					(*aVEL) = K * ((*aVEL) +
							F1 * ((*aP_BEST) - (*adPUB)) +
							F2 * ((*aL_BEST) - (*adPUB)) +
							F3 * ((*aG_BEST) - (*adPUB)));
						 if((*aVEL) > ( 20)){(*aVEL) =( 20);}
					else if((*aVEL) < (-20)){(*aVEL) =(-20);}
					(*adPUB) += (*aVEL);
						 if ((*adPUB) < aSNR[j]) { (*adPUB) = aSNR[j];}
					else if ((*adPUB) > uSNR[j]) { (*adPUB) = uSNR[j];}
					aVEL ++;
					adPUB ++;
					aP_BEST ++;
					aL_BEST ++;
					aG_BEST ++;
				}
			}else {
				for (MY::uint j = 0; j < SON; j++) {Child[id][j] = dPUB[i][j];}

				if (U > mut_rate) {
					MY::uint m;
					do { m = dist_I_PSO_DIM(mt_A_PSO_DIM[id]); } while (i == m);
					double cr_d =  DoubleFromBits(mt_A_PSO_DIM[id]());
					double S	= (DoubleFromBits(mt_A_PSO_DIM[id]())-0.5)*20;
					for (MY::uint j = aDIM; j < SON; j++) {						
						Child[id][j] = (cr_d)*dPUB[i][j] + (1 - cr_d) * dPUB[m][j] + S;
					}
				}else{
					for (MY::uint j = aDIM; j < SON; j++){
						Child[id][j] = aSNR[j] + araSNR[j]* DoubleFromBits(mt_A_PSO_DIM[id]());
					}
				}
				for (MY::uint j = aDIM; j < SON; j++) {
						 if (Child[id][j] < aSNR[j]) { Child[id][j] = aSNR[j];}
					else if (Child[id][j] > uSNR[j]) { Child[id][j] = uSNR[j];}
				}
				func->UYGUNLUK(Child[id],Child[id][nDIM],false);
				if (dPUB[i][nDIM]>Child[id][nDIM]){
					for (MY::uint j = aDIM; j < SON; j++){dPUB[i][j] = Child[id][j];}
				}
			}
			func->UYGUNLUK(dPUB[i],dPUB[i][nDIM],false);
			if (P_BEST[i][nDIM] > dPUB[i][nDIM]){
				for (MY::uint j = 0; j <= nDIM; j++) {P_BEST[i][j] = dPUB[i][j]; }
				if (G_BEST[id][nDIM] > P_BEST[i][nDIM]) {
					for (MY::uint j = 0; j <= nDIM; j++){G_BEST[id][j] = P_BEST[i][j]; }
	//				#pragma omp critical
	//				{
	//					if (G_BEST[0][nDIM] > G_BEST[id][nDIM]) {
	//						for (MY::uint j = 0; j <= nDIM; j++){G_BEST[0][j] = G_BEST[id][j]; }
	//					}
	//				}
				}
			}
			sum[id] += P_BEST[i][nDIM];
			sq_sum[id] += (P_BEST[i][nDIM]*P_BEST[i][nDIM]);
			for (MY::uint j = aDIM; j < SON; j++) {P_MEAN[id][j]+=dPUB[i][j];}
		}
	}
}
/***********************************************************************************/
double std_dev2(double a[], int n) {
	if(n == 0){return 0.0;}
    double sum = 0;
    double sq_sum = 0;
    for(int i = 0; i < n; ++i) {
       sum += a[i];
       sq_sum += a[i] * a[i];
    }
    double mean = sum / n;
    double variance = sq_sum / n - mean * mean;
    return sqrt(variance);
}
/***********************************************************************************/
bool PSO_DIM::CRITERIA(const bool verbose) {
	const	double eps = 1E-5;
	static	double old_mean = 0;
	static	double old_sd = 0;
	for (MY::uint i = 1; i < nProcessors; i++) {
		sum[0] += sum[i];
		sq_sum[0] += sq_sum[i];
	}
	double tmp=1.0/(double)nPUB;
	double mean = sum[0] * tmp;
    double variance = (sq_sum[0] * tmp)- (mean * mean);
	variance = variance>0 ? variance:0;
    double sd =  sqrt(variance);

	if(verbose){std::cout<<"M "<<mean<<" S "<<sd<<"\n";}

	if(mean<(G_BEST[0][nDIM]-0.0001)){system("pause");}

	if ((fabs(old_sd   - sd  ) < eps &&
		 fabs(old_mean - mean+100) < eps) ||
		sd < fabs(mean)*0.001) {return true;}
	old_mean = mean;
	old_sd = sd;
	return false;
}
/***********************************************************************************/
void PSO_DIM::read_OPT() {
	func->UYGUNLUK(G_BEST[0],G_BEST[0][nDIM],false);
	double *TEST	= new double[nDIM+1];
	double *BEST	= new double[nDIM+1];
	for (MY::uint j = 0; j <= nDIM; j++) {BEST [j] = G_BEST[0][j];}
	std::fstream DOSYA;
	std::string NAME[2] = {	"old_PSO_OPT.txt",
							"PSO_OPT.txt"};
	for(MY::uint i = 0; i < 2; i++) {
		DOSYA.open(PATH + NAME[i], std::ios::in);
		if(!DOSYA.is_open()){std::cout<<PATH+NAME[i]<<" not opened\n";}else{
			std::vector<double> vTmp;
			double dTmp;
			std::string line;
			while (std::getline(DOSYA, line)){
				std::istringstream iss(line);
				while(iss>>dTmp){vTmp.push_back(dTmp);}
				for(MY::uint j=0;j<nDIM;j++){
					TEST[j]=vTmp[j];
						 if( TEST[j] < aSNR[j]){TEST[j] = aSNR[j];}
					else if( TEST[j] > uSNR[j]){TEST[j] = uSNR[j];}
				}
				func->UYGUNLUK(TEST,TEST[nDIM],false);
				if (BEST[nDIM] > TEST[nDIM]) {for (MY::uint j = 0; j <= nDIM; j++) {BEST [j] = TEST[j];}}
				vTmp.clear();
			}
			DOSYA.close();
		}
	}
	for (MY::uint j = 0; j <= nDIM; j++) {G_BEST[0][j] = BEST[j];}
	func->UYGUNLUK(G_BEST[0],G_BEST[0][nDIM],false);
	if(G_BEST[0][nDIM] != BEST[nDIM]){std::cout<<"HATA\n";system("pause");}
	delete []TEST;
	delete []BEST;
	std::cout<<"read_OPT done\n";
}
/***********************************************************************************/
double PSO_DIM::RUN(double* EN_IYI) {
	std::cout << "running ..." << std::endl;
	bool use_gaussian=false;
	double old;
	std::string NAME_1="PSO_OPT.txt";
	MY::uint nTEST = 1000;
	MY::uint nDEL  = 100;
	MY::uint iS    = 0;
	MY::uint iE    = nTEST+iS;
	do{
		std::uint64_t AA=static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		for(MY::uint i=0;i<nProcessors;i++){mt_A_PSO_DIM[i]=Xoshiro256PlusPlus(AA+i*10+rand()); mt_A_PSO_DIM[i].jump() ;}

		if(iE>=func->nDATA){iE=(func->nDATA-1);}
		func->SET_LIM(iS,iE);
	//	func->INIT_RBF(func->nILK);
		func->GET_SNR(aSNR, uSNR);
		for (MY::uint j = 0; j < nDIM; j++){
			aSNR[j] -= 50;
			uSNR[j] += 50;
			araSNR[j]= uSNR[j]-aSNR[j];
		}
		use_gaussian=false;
		MY::uint stage=0;
		read_OPT();
		do {
			old=G_BEST[0][nDIM];
			for (aDIM = 0; aDIM < nDIM; aDIM+=LDIM) {
				init_POP(use_gaussian);
				UPDATE_LG_BEST();
				for (MY::uint iter = 0; iter <= iterMAX; iter++) {
					double old1=G_BEST[0][nDIM];
					URET();
					UPDATE_LG_BEST();
					if (iter % 10 == 0 || fabs(G_BEST[0][nDIM]-old1)>1e-5)
					{
						std::cout << iS << "\t"
								  << iter << "\t"
								  << "stg: " << stage << "\t"
								  << "DIM: " << aDIM << "\t"
							      <<  G_BEST[0][nDIM] << "\t";
						if (CRITERIA(true)) { break; }
					}
					if (iter % 20 == 0){inject_POP();}
				}
			}
			use_gaussian=true;
			stage++;
		}while(fabs(G_BEST[0][nDIM]-old)>1E-6 || stage<2);
		std::cout<<func->nDATA<<"\t"<<func->nILK<<"\t"<<func->nSON<<"\t"<<G_BEST[0][nDIM]<<"\n";
		func->UYGUNLUK(G_BEST[0],G_BEST[0][nDIM],true);
		func->YAZ_OPT (PATH+NAME_1,G_BEST[0]);
		std::cout<<"____________________________\n";
		if(iE >=(func->nDATA-1)){break;}
		iS+=nDEL;
		iE+=nDEL;
	}while(iS<(func->nDATA));

	for (MY::uint i = 0; i < nDIM; i++) { EN_IYI[i] = G_BEST[0][i]; }
	return G_BEST[0][nDIM];
}
/*******************************************************************************/
void PSO_DIM::init_POP(const bool& use_gaussian) {
//	std::cout<<"init_POP started\n";
	MY::uint EK=0;
	if(use_gaussian==false){
		std::fstream DOSYA;
		std::string NAME[2] = {	"old_PSO_OPT.txt",
								"PSO_OPT.txt"};
		for(MY::uint i = 0; i < 2; i++) {
			DOSYA.open(PATH + NAME[i], std::ios::in);
			if(!DOSYA.is_open()){std::cout<<PATH+NAME[i]<<" not opened\n";}else{
				std::vector<double> vTmp;
				double dTmp;
				std::string line;
				MY::uint LINE_NO=0;
				while (EK<nPUB && std::getline(DOSYA, line)){
					std::istringstream iss(line);
					while(iss>>dTmp){vTmp.push_back(dTmp);}
					MY::uint iTMP = MY::uint(vTmp.size());
					if (i==1){iTMP-=3;}
					LINE_NO++;
					if(nDIM != iTMP){
							std::cout<<"SIZE MISMATCH\n";
							std::cout<<i<<"\t"<<NAME[i]<<"\t"<<LINE_NO<<"\n";
							std::cout << nDIM <<"\t"<< iTMP <<"\n";
							std::cout<<line <<"\n";
/*					#if defined(_MSC_VER)
						system("pause");
					#elif defined(__GNUC__)
						int sil=0;
						std::cout<<"press 1 to continue\n";
						std::cin>>sil;
					#endif
					*/
					}				
					if (iTMP>nDIM){iTMP=nDIM;}
					for(MY::uint j=0;j<iTMP;j++){dPUB[EK][j]=vTmp[j];}
//					MY::uint s=EK;
//					if(TEKRAR(dPUB,dPUB[EK],0, s,nPUB,nDIM)==true){EK--;}
					func->UYGUNLUK(dPUB[EK],dPUB[EK][nDIM],false);
					for (MY::uint j = 0; j <= nDIM; j++) {P_BEST[EK][j] = dPUB[EK][j];}
					if (G_BEST[0][nDIM] > dPUB[EK][nDIM]) {
						for(MY::uint k=0;k<nProcessors;k++){
							for (MY::uint j = 0; j <= nDIM; j++) {G_BEST[k][j] = dPUB[EK][j];}
						}
					}
					vTmp.clear();
					EK++;
					if(EK>=nPUB){break;}
				}
				DOSYA.close();
			}
			if(EK>=nPUB){break;}
		}
	}
	UPDATE_LG_BEST();
	MY::uint SON=aDIM+LDIM;

	for (MY::uint i = 0; i < EK; i++) {
		for (MY::uint j = 0; j < nDIM; j++) {VEL   [i][j] = 0;}

		for (MY::uint j = 0; j < aDIM; j++) {
			dPUB  [i][j] = G_BEST[0][j];
			P_BEST[i][j] = G_BEST[0][j];
		}
		for (MY::uint j = SON; j <= nDIM; j++) {
			dPUB  [i][j] = G_BEST[0][j];
			P_BEST[i][j] = G_BEST[0][j];
		}
	}

	#pragma omp parallel
	{
		MY::uint nRTH = (MY::uint)omp_get_num_threads();
		MY::uint id   = (MY::uint)omp_get_thread_num();
		if(id>=nProcessors){std::cout<<id<<"\t"<<nProcessors<<"\tTHREAD SAYI HATA\n";system("pause");}
		for (MY::uint i = id+EK; i < nPUB; i+=nRTH) {
			if(i>=nPUB){break;}
			for (MY::uint j = 0; j <  nDIM; j++) {VEL   [i][j] =0;}
			for (MY::uint j = 0; j <= nDIM; j++) {
				dPUB  [i][j] = G_BEST[0][j];
				P_BEST[i][j] = G_BEST[0][j];
			}

			for(MY::uint j=aDIM;j<SON;j++){
				MY::uint nTRY=0;
				do{
					dPUB[i][j] = G_BEST[0][j] + 100*(DoubleFromBits(mt_A_PSO_DIM[id]())-0.5);
					nTRY++;
					if(nTRY>=20){dPUB[i][j] = aSNR[j] + araSNR[j]*DoubleFromBits(mt_A_PSO_DIM[id]());}
				}while(dPUB[i][j] < aSNR[j] || dPUB[i][j] > uSNR[j]);
			}
			func->UYGUNLUK(dPUB[i],dPUB[i][nDIM],false);
			for (MY::uint j = 0; j <= nDIM; j++) {P_BEST[i][j] = dPUB[i][j];}
			if (G_BEST[id][nDIM] > dPUB[i][nDIM]) {
				for (MY::uint j = 0; j <= nDIM; j++) {G_BEST[id][j] = dPUB[i][j];}
			}
		}
	}
	UPDATE_LG_BEST();
	std::cout<<"init_POP done\n";	
}
/***********************************************************************************/