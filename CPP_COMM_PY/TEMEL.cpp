#include <stdio.h>  
#include <Winsock2.h>  
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
int main()
{
	//	std::cout << std::setprecision(3);
	std::cout << std::setprecision(std::numeric_limits<double>::digits10 + 1);

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return -1;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8888);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	std::string baseCh = "hello";
	char recvBuf[50];
	std::string V1;
	V1.clear();
	double Value;
	for (int i = 0; i < 50; i++) { recvBuf[i] = ' '; }
	for (int clc = 0; clc < 5; clc++)
	{
		send(sockClient, baseCh.c_str(), strlen(baseCh.c_str()) + 1, 0);
		recv(sockClient, recvBuf, 50, 0);
		Value = atof(recvBuf);
		for (int i = 0; i < 50; i++) {
			V1 += recvBuf[i];
			recvBuf[i] = ' ';
		}
		std::cout << "FROM PYTHON: " << V1 << std::endl;
		std::cout << "FROM PYTHON: " << Value << std::endl;
		V1.clear();
		std::cin >> baseCh;
	}

	closesocket(sockClient);
	WSACleanup();
	system("pause");
	return 0;
}