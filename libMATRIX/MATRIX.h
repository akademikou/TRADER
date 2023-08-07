#include <iostream>
#ifndef MATRIX_H
#define MATRIX_H
	/**************************************************************/
#define ALLOC
#ifndef ALLOC
#include <Windows.h>
#include <Memoryapi.h>
	/**************************************************************/
template<class Tipus> Tipus * Init_Matrix_1D(const unsigned int& size_m1) {
		void* mem1 = VirtualAlloc(NULL, size_m1 * sizeof(Tipus), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // PAGE_EXECUTE_READWRITE
		if (mem1 == NULL) { std::cout << size_m1 << "  mem error" << std::endl; return NULL; }
		VirtualLock(mem1 ,size_m1*sizeof(Tipus));
		return (Tipus *)mem1;
	};
	/**************************************************************/
template<class Tipus> Tipus * Free_Matrix_1D(Tipus *X) {
		if (!X) { return NULL; }
		VirtualFree(X, 0, MEM_RELEASE);
		return NULL;
	};
	/**************************************************************/
	/**************************************************************/
template<class Tipus> Tipus ** Init_Matrix_2D(const unsigned int &nx, const unsigned int &ny){
		Tipus **X;
		Tipus  *p;
		unsigned int i,s = nx*ny;
		X = (Tipus **)VirtualAlloc(NULL, nx * sizeof(Tipus *), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if(X==NULL){return NULL;}
		VirtualLock(X ,nx*sizeof(Tipus *));
		p = (Tipus  *)VirtualAlloc(NULL, s * sizeof(Tipus), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if(p == NULL){
			VirtualFree(X, 0, MEM_RELEASE);
			free(X);
			return NULL;
		}
		VirtualLock(p ,s*sizeof(Tipus));
		for(i=0;i<nx;i++){X[i]=p; p+=ny;}
		return X;
	};
	/**************************************************************/
template<class Tipus> Tipus ** Free_Matrix_2D(Tipus **X) {
		if (!X) { return NULL; }
		VirtualFree(X[0], 0, MEM_RELEASE);
		VirtualFree(X, 0, MEM_RELEASE);
		return NULL;
	};
	/**************************************************************/
#else
	/**************************************************************/
template<class Tipus> Tipus * Init_Matrix_1D(const unsigned int& size_m1) {
		void* mem1 = calloc(size_m1, sizeof(Tipus));
//		void* mem1 = new Tipus[size_m1];
		if (mem1 == NULL) { std::cout << size_m1 << " Init_Matrix_1D mem error" << std::endl; return NULL; }
		return (Tipus *)mem1;
	};
	/**************************************************************/
template<class Tipus> Tipus * Free_Matrix_1D(Tipus *X) {
		if (!X) { return NULL; }
		free(X);
//		delete X;
		return NULL;
	};
	/**************************************************************/
template<class Tipus> Tipus ** To_2D(const int &nx, const unsigned int& ny,Tipus  *p){
		Tipus **X;
		unsigned int i,s = nx*ny;
		X = (Tipus **)calloc(nx,sizeof(Tipus *));
		if(X==NULL){return NULL;}
		if(p == NULL){
			free(X);
			return NULL;
		}
		for(i=0;i<nx;i++){X[i]=p; p+=ny;}
		return X;
	};
	/**************************************************************/
	/**************************************************************/
template<class Tipus> Tipus ** Init_Matrix_2D(const unsigned int &nx, const unsigned int& ny){
		Tipus **X;
		Tipus  *p;
		long long int i,s;
		long long int N1 = nx;
		long long int N2 = ny;
		s=N1*N2;
//		std::cout<<nx<<"\t"<<ny<<"\t"<<nx*ny<<"\t"<<N1*N2<<"\t"<<s<<"\n";
		X = (Tipus **)calloc(nx,sizeof(Tipus *));
		if(X==NULL){ std::cout << nx << " Init_Matrix_2D_1 mem error" << nx<<"\t"<<ny<<std::endl; return NULL;}
		p = (Tipus  *)calloc(s,sizeof(Tipus  ));
//		std::cout<<nx<<"\t"<<ny<<"\t"<<s<<"\n";
		if(p == NULL){
			free(X);
			std::cout << ny << "  Init_Matrix_2D_2 mem error " << nx<<"\t"<<ny<<std::endl;
			return NULL;
		}
		for(i=0;i<nx;i++){X[i]=p; p+=ny;}
//		long long BOYUT1 = nx*sizeof(Tipus *)/(1024*1024);
//		long long BOYUT2 = s*sizeof(Tipus  )/(1024*1024);
//		std::cout<<"BOYUT "<<BOYUT1<<"\t"<<BOYUT2<<"\t"<<BOYUT1+BOYUT2<<"\n";
	//	int ii;std::cin>>ii;
		return X;
	};
	/**************************************************************/
template<class Tipus> Tipus ** Free_Matrix_2D(Tipus **X) {
		if (!X) { return NULL; }
		free(X[0]);
		free(X);
		return NULL;
	};
	/**************************************************************/
#endif

template<class Tipus> Tipus *** To_3D(const unsigned int& nx, const unsigned int& ny, const unsigned int& nz,Tipus* p){
		Tipus ***X;
		unsigned int i, j, ii;
		unsigned int s = nx*ny*nz;
		X=(Tipus ***)calloc(nx,sizeof(Tipus **));
		if (X==NULL){return NULL;}
		for (i=0; i<nx; i++){
			X[i]=(Tipus **)calloc(ny,sizeof(Tipus *));
			if (X[i]==NULL){
				for (ii=0; ii<i; ii++){free(X[ii]);}
				free(X);
				return NULL;
			}
		}
		if (p == NULL){
			for (i = 0; i< nx; i++){free(X[i]);}
			free(X);
			return NULL;
		}
		for (i=0; i<nx; i++){
			for (j=0; j<ny; j++){
				X[i][j] = p;
				p += nz;
			}
		}
		return X;
	};
	/**************************************************************/

template<class Tipus> Tipus *** Init_Matrix_3D(const unsigned int& nx, const unsigned int& ny, const unsigned int& nz){
		Tipus ***X;
		unsigned int i, j, ii;
		unsigned int s = nx*ny*nz;
//		X=(Tipus ***)VirtualAlloc(0, nx * sizeof(Tipus **), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		X=(Tipus ***)calloc(nx,sizeof(Tipus **));
		if (X==NULL){return NULL;}
//		VirtualLock(X, nx * sizeof(Tipus *));
		for (i=0; i<nx; i++){
//			X[i]=(Tipus **)VirtualAlloc(0, ny * sizeof(Tipus *), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			X[i]=(Tipus **)calloc(ny,sizeof(Tipus *));
			if (X[i]==NULL){
				for (ii=0; ii<i; ii++){
//					VirtualFree(X[ii], 0, MEM_RELEASE);
					free(X[ii]);
				}
//				VirtualFree(X, 0, MEM_RELEASE);
				free(X);
				return NULL;
			}
//			VirtualLock(X[i], ny * sizeof(Tipus *));
		}
//		Tipus* p = (Tipus*)VirtualAlloc(0, s * sizeof(Tipus), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		Tipus* p = (Tipus*)calloc(s, sizeof(Tipus));
		if (p == NULL){
			for (i = 0; i< nx; i++){
//				VirtualFree(X[i], 0, MEM_RELEASE);
				free(X[i]);
			}
//			VirtualFree(X, 0, MEM_RELEASE);
			free(X);
			return NULL;
		}
//		VirtualLock(p, s * sizeof(Tipus));
		for (i=0; i<nx; i++){
			for (j=0; j<ny; j++){
				X[i][j] = p;
				p += nz;
			}
		}
		return X;
	};
	/**************************************************************/
template<class Tipus> Tipus *** Free_Matrix_3D(Tipus ***X, unsigned int nx) {
		if (!X) { return NULL; }
//		VirtualFree(X[0][0], 0, MEM_RELEASE);
		free(X[0][0]);
		for (unsigned int i = 0; i<nx; i++) {
//			VirtualFree(X[0], 0, MEM_RELEASE);
			free(X[i]);
		}
//		VirtualFree(X, 0, MEM_RELEASE);
		free(X);
		return NULL;
	};
	/**************************************************************/
#endif
