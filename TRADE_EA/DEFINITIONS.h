#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_
#define	T_STAMP	0
#define	Op		1
#define	Hi		2
#define	Lo		3
#define	Cl		4
#define	Volume	5
#define	ClCl	6
#define	MAX_RET	7
#define	PCD		8
#define	EMAC	9
#define	SMAC	10
#define	RSIC	11
#define	EMA		12
#define	SMA		13
#define	SAR		14
#define	BB_MID	15
#define	RS		16
#define	PC		17
#define	HISTOGRAM	18
#define	OBV		19
#define	ROC		20
#define	STOCH	21
#define	RSI		22
#define	WPR		23
#define	MFI		24
#define	CCI		25
#define	ATR		26
#define	ADX		27
#define	ARN		28
#define	STD		29
#define	OSC		30
#define	oEMAC	31
#define	oSMAC	32
#define	oRSEC	33
#define	oRSIC	34
#define	oSTC	35
#define	oRSI	36
#define	oWPR	37
#define	oMFI	38
#define	oPCZ	39
#define	oADX	40
#define	oARN	41
#define	oOSC	42
#endif /* DEFINITIONS_H_ */
/***********************************************/
#ifndef PATH
#if defined(_MSC_VER)    //  Microsoft
// 		#define PATH "C:/Users/akade/Documents/R_PROJ_LVQ/"
		#define PATH ""
	#elif defined(__GNUC__)   //  GCC
//		#define PATH "/root/R_PROJ_LVQ/"
#define PATH "/home/akademikou/R_PROJ_LVQ/"
#endif
#endif
/***********************************************/
