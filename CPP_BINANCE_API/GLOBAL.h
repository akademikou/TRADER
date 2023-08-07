//#pragma once

//#define API_KEY 		"api key"
//#define SECRET_KEY	"user key"
// https://testnet.binance.vision/api
//#define API_KEY		"rEdAeoTHwSSg1oynXL95xtA3JwdqsL0e6x1KSC7Kf2k9CSveBd84sLvh0Qq3VfQO"
//#define SECRET_KEY	"aKJcn5UuFZBNGeYVlEDLsqBgm2pzVBJwDbXMpMRPqo7sLCzEm6b2DJ2ZzDAuZxxJ"
//#define API_KEY 		"HUuXiG6wOd6tcqeUhOexkres6R5ASKzG2F6uFnbbQtLnvrauoiYhpsMWQb65A5Q0"
//#define SECRET_KEY		"mqUTY6qGNAhfG7wlL6kz4mUXGNCC77qQS0aAvuGEOEh1ECwubbszRyE8YqVLn5mV"

/*
 \033 is the ESC

			foreground background
black        30         40
red          31         41
green        32         42
yellow       33         43
blue         34         44
magenta      35         45
cyan         36         46
white        37         47

reset             0  (everything back to normal)
bold/bright       1  (often a brighter shade of the same colour)
underline         4
inverse           7  (swap foreground and background colours)
bold/bright off  21
underline off    24
inverse off      27
*/


#define Back_BLACK   "\033[0;40m"
#define Back_RED   "\033[0;41m"
#define Back_GREEN   "\033[0;42m"

#define RESET   "\033[0m"
#define BLACK   "\033[0;30m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"

#define BOLDBLACK   "\033[1;30m"
#define BOLDRED     "\033[1;31m"
#define BOLDGREEN   "\033[1;32m"
#define BOLDYELLOW  "\033[1;33m"
#define BOLDBLUE    "\033[1;34m"
#define BOLDMAGENTA "\033[1;35m"
#define BOLDCYAN    "\033[1;36m"
#define BOLDWHITE   "\033[1;37m"


std::map < std::string, std::map <double, double> >  depthCache;
std::map < long, std::map <std::string, double> >  klinesCache;
std::map < long, std::map <std::string, double> >  aggTradeCache;
std::map < long, std::map <std::string, double> >  userTradeCache;
std::map < std::string, std::map <std::string, double> >  userBalance;

int lastUpdateId;