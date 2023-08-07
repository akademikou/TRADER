namespace MY {typedef unsigned int uint;}
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <cmath>
#include "xoshiro.h"
using namespace XoshiroCpp;

#include "MATRIX.h"
const int		EMA_LIM=501;
#include "CLASS_BR.h"
#include "BR_MAIN.h"

/***********************************************************************************/
int main(){

	  std::cout
    << "char________________: \t"<<static_cast< int >(std::numeric_limits< char >::max()) << "\n"
    << "unsigned char_______: \t"<<static_cast< int >(std::numeric_limits< unsigned char >::max()) << "\n"
    << "short_______________: \t"<<std::numeric_limits< short >::max() << "\n"
    << "unsigned short______: \t"<<std::numeric_limits< unsigned short >::max() << "\n"
    << "int_________________: \t"<<std::numeric_limits< int >::max() << "\n"
    << "unsigned int________: \t"<<std::numeric_limits< unsigned int >::max() << "\n"
    << "long________________: \t"<<std::numeric_limits< long >::max() << "\n"
    << "unsigned long_______: \t"<<std::numeric_limits< unsigned long >::max() << "\n"
    << "long long___________: \t"<<std::numeric_limits< long long >::max() << "\n"
    << "unsigned long long__: \t"<<std::numeric_limits< unsigned long long >::max() << "\n";
	system("pause");
//	system("cls");
	printf ("\033c");
//	alias cls='printf "\033c"'
	std::fstream DOSYA;
	std::string NAME_1="OPT_PAR.txt";
	DOSYA.open(PATH+NAME_1,std::ios::out);

	DOSYA.close();

	int nTEST=100;
	BR_PAR **G_PAR= Init_Matrix_2D<BR_PAR>(11,nTEST);
	RUN_BR(1,"ADX","ADX" ,1, 1,G_PAR[0]);
	RUN_BR(1,"ARN","ARN" ,1, 1,G_PAR[1]);
	RUN_BR(1,"RSI","RSI" ,1, 1,G_PAR[2]);
	RUN_BR(1,"WPR","WPR" ,1, 1,G_PAR[3]);
	RUN_BR(1,"MFI","MFI" ,1, 1,G_PAR[4]);
	RUN_BR(2,"STC","STC" ,1, 1,G_PAR[5]);
	RUN_BR(2,"EMA","EMAC",1, 1,G_PAR[6]);
	RUN_BR(2,"SMA","SMAC",1, 1,G_PAR[7]);
	RUN_BR(2,"RSE","RSEC",1, 1,G_PAR[8]);
	RUN_BR(2,"RSI","RSIC",1, 1,G_PAR[9]);
	RUN_BR(4,"EMA","PCY" ,5,10,G_PAR[10]);
	return 0;
}
/***********************************************************************************/
