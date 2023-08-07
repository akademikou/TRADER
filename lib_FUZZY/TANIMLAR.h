#pragma once
#ifndef TANIMLAR_H
#define TANIMLAR_H

#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#define BOY_1D 50

const int TRI	=1;
const int TRAP	=2;
const int GAUSS	=3;
const int BELL	=4;
const int SIG	=5;

const int AND = -100;
const int OR  = -101;
const int NOT = -102;
const int OUT = -103;

#endif
/**********************************************************************************************/