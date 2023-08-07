#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include "MATRIX.h"
#include "CLASS_FLC.h"
const double EPS=1E-15;
/*******************************************************************************/
FLC::~FLC() {
	if(X !=NULL){Free_Matrix_2D(X); X=NULL;}
	if(C !=NULL){Free_Matrix_2D(C); C=NULL;}
	if(D !=NULL){Free_Matrix_2D(D); D=NULL;}
	if(W !=NULL){Free_Matrix_2D(W); W=NULL;}
	if(J !=NULL){Free_Matrix_1D(J); J=NULL;}
	if(SC!=NULL){Free_Matrix_2D(SC);SC=NULL;}
}
/*******************************************************************************/
FLC::FLC(unsigned int _n, unsigned int _c,unsigned int _b,
	double **_X) {
	ITER_MAX = 1000;
	n = _n;
	c = _c;
	dim = _b;
	m = 2.5;
//	X=_X;
	X = Init_Matrix_2D<double>(n, dim);
	C = Init_Matrix_2D<double>(c, dim);
	D = Init_Matrix_2D<double>(n, c);
	W = Init_Matrix_2D<double>(n, c);
	J = Init_Matrix_1D<double>(ITER_MAX);
	SC= Init_Matrix_2D<double>(dim,2);

	for (unsigned int d=0;d<dim;d++){

		double mean=0;
		double sd=0;

		for (unsigned int i=0;i<n;i++){mean+=_X[i][d];}
		mean/=double(n);

		for (unsigned int i=0;i<n;i++){sd+=((_X[i][d]-mean)*(_X[i][d]-mean));}
		sd=sqrt(sd/double(n));

		for (unsigned int i=0;i<n;i++){
			X[i][d]=(_X[i][d]-mean);
			if(sd>0){X[i][d]/=sd;}
		}
		SC[d][0]=mean;
		SC[d][1]=sd;

	}

	for (unsigned int i=0;i<10;i++){
	//	for (unsigned int d=0;d<dim;d++){std::cout<<X[i][d]<<"\t";}
	//	std::cout<<"\n";
	//	for (unsigned int d=0;d<dim;d++){std::cout<<_X[i][d]<<"\t";}
	//	std::cout<<"\n";
	}

	for (unsigned int i=0;i<ITER_MAX;i++){J[i]=0;}

	for (unsigned int i=0;i<n;i++){
		for (unsigned int j=0;j<c;j++){
			D[i][j]=0;
			W[i][j]=double(rand())/double(RAND_MAX);
		}
	}
	for (unsigned int d=0;d<dim;d++){
		for (unsigned int i=0;i<c;i++){C[i][d]=0;}
	}

	for (unsigned int d=0;d<dim;d++){
		double ENB=X[0][d];
		double ENK=X[0][d];

		for (unsigned int i=0;i<n;i++){
			if(ENK>X[i][d]){ENK=X[i][d];}
			if(ENB<X[i][d]){ENB=X[i][d];}
		}

		for (unsigned int i=0;i<c;i++){
			C[i][d]=ENK+(ENB-ENK)*double(i)/double(c);
//			std::cout<<C[i][d]<<"\t";
		}
//		std::cout<<"\n";
	}

	for (unsigned int i=0;i<c;i++){
		unsigned int ind=rand()%n;
		for (unsigned int d=0;d<dim;d++){C[i][d]=X[ind][d];}
	}
	HESAP_D();
	HESAP_WF();
}
/***********************************************************************************/
void FLC::HESAP_J(unsigned int iter){
	J[iter]=0;	// YAKINSAMA_FONKS
	for (unsigned int i=0;i<n;i++){
		for (unsigned int j=0;j<c;j++)
		{J[iter]+=pow(W[i][j],m)*D[i][j]*D[i][j];}
	}
}
/***********************************************************************************/
void FLC::HESAP_D(){
	for (unsigned int i=0;i<n;i++){
		for (unsigned int j=0;j<c;j++){
			double sum=0;
			for (unsigned int d=0;d<dim;d++)
			{sum+=((X[i][d]-C[j][d])*(X[i][d]-C[j][d]));}
			D[i][j]=sqrt(sum);
			if(D[i][j]<EPS){D[i][j]=EPS;}
		}
	}
}
/***********************************************************************************/
void FLC::HESAP_CF(){
	for (unsigned int d=0;d<dim;d++){//
		for (unsigned int j=0;j<c;j++){
			double ust=0;
			double alt=0;
			for (unsigned int i=0;i<n;i++){
				double S=pow(W[i][j],m);
				ust+=(S*X[i][d]);
				alt+=S;
			}
			if(alt<EPS){alt=EPS;}
			C[j][d]=ust/alt;
		}
	}
}
/***********************************************************************************/
void FLC::HESAP_CK(){
	for (unsigned int d=0;d<dim;d++){
		for (unsigned int j=0;j<c;j++){
			double ust=0;
			double alt=0;
			for (unsigned int i=0;i<n;i++){
				ust+=(W[i][j]*X[i][d]);
				alt+=(W[i][j]);
			}
			if(alt<EPS){alt=EPS;}
			C[j][d]=ust/alt;
		}
	}
}
/***********************************************************************************/
void FLC::HESAP_WF(){
	double tmp=(2.0/(m-1));
	for (unsigned int i=0;i<n;i++){
		for (unsigned int j=0;j<c;j++){
			double sum=0;
			for (unsigned int q=0;q<c;q++)
			{sum+=pow((D[i][j]/D[i][q]),tmp);}
			if(sum<EPS){sum=EPS;}
			W[i][j]=1/sum;
		}
	}
}
/***********************************************************************************/
void FLC::HESAP_WK(){
	for (unsigned int i=0;i<n;i++){
		unsigned int M=0;
		double ENK=D[i][0];
		for (unsigned int j=0;j<c;j++){
			if(ENK>D[i][j]){
				ENK=D[i][j];
				M=j;
			}
			W[i][j]=0;
		}
		W[i][M]=1;
	}
}
/***********************************************************************************/
void FLC::RUN() {
	RUN_K();
	RUN_F();

	for (unsigned int d=0;d<dim;d++){
		double mean=SC[d][0];
		double sd  =SC[d][1];
		for (unsigned int i=0;i<c;i++){
			if(sd>0){C[i][d]*=sd;}
			C[i][d]+=mean;
		}
		for (unsigned int i=0;i<n;i++){
			if(sd>0){X[i][d]*=sd;}
			X[i][d]+=mean;
		}
	}

	for (unsigned int i=0;i<10;i++){
		for (unsigned int d=0;d<dim;d++){std::cout<<X[i][d]<<"\t";}
		std::cout<<"\n";
	}

	HESAP_D();
	HESAP_WF();
}
/***********************************************************************************/
void FLC::RUN_F() {
	for (unsigned int iter=1;iter<ITER_MAX;iter++){
		HESAP_WF();
		HESAP_CF();
		HESAP_D();
		HESAP_J(iter);
//		std::cout<<iter<<"\t"<<J[iter]<<std::endl;
		if(J[iter]==J[iter-1]){
			std::cout<<"CONDITION FLC "<<iter<<std::endl;
			break;
		}
	}
}
/***********************************************************************************/
void FLC::RUN_K() {
	for (unsigned int iter=1;iter<ITER_MAX;iter++){
		HESAP_WK();
		HESAP_CK();
		HESAP_D();
		HESAP_J(iter);
//		std::cout<<iter<<"\t"<<J[iter]<<std::endl;
		if(J[iter]==J[iter-1]){
			std::cout<<"CONDITION FLC "<<iter<<std::endl;
			break;
		}
	}
}
/***********************************************************************************/
void FLC::YAZ(int id) {
	std::string POST=".txt";
	std::string MED=std::to_string(id);
	std::string NAME_1="W";
	std::string NAME_2="C";
	std::fstream DOSYA;

	DOSYA.open(PATH+NAME_1+MED+POST,std::ios::out);
	for (unsigned int i=0;i<n;i++){
		DOSYA<<W[i][0];
		for (unsigned int j=1;j<c;j++){DOSYA<<"\t"<<W[i][j];}
		DOSYA<<"\n";
	}
	DOSYA.close();

	DOSYA.open(PATH+NAME_2+MED+POST,std::ios::out);
	for (unsigned int i=0;i<c;i++){
		DOSYA<<C[i][0];
		for (unsigned int d=1;d<dim;d++){DOSYA<<"\t"<<C[i][d];}
		DOSYA<<"\n";
	}
	DOSYA.close();
}
/***********************************************************************************/
