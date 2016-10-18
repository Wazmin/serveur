// TestReseau1.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")


bool InitializeSockets()
{
	#if PLATFORM == PLATFORM_WINDOWS
		WSADATA WsaData;
		return WSAStartup(MAKEWORD(2, 2),&WsaData)== NO_ERROR;
	#else
		return true;
	#endif
}

void ShutdownSockets()
{
	#if PLATFORM == PLATFORM_WINDOWS
		WSACleanup();
	#endif
}

int TransformeAdresse(char adresse[]) {
	char tmpChar[5];
	int i = 0;
	while (tmpChar[i] != '.' || tmpChar[i] != '/o') {

	}
}

int main()
{
	bool serveurOn = false;
	InitializeSockets();

	printf("\nConfiguration du socket");
	int handle = socket(AF_INET,
						SOCK_DGRAM,
						IPPROTO_UDP);
	
	printf("\nTest du socket : ");
	if (handle <= 0)
	{
		printf("\nvaleur de handle : %i ", handle);
		printf("\nechec de creation du socket!");
		return false;
	}
	printf("reussi!");

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( (unsigned short) 3000 );

	if (bind(handle,
		(const sockaddr*)&address,
		sizeof(sockaddr_in)) < 0) 
	{
		printf("\nechec du bind socket!");
		return false;
	}
	printf("\nBind reussi");


	DWORD nonBlocking = 1;
	if (ioctlsocket(handle,
		FIONBIO,
		&nonBlocking) != 0)
	{
		printf("\nechec du parametrage non-blocking");
		return false;
	}
	printf("\nParametrage non-blocking reussi");
	serveurOn = true;
	while (serveurOn) {



	}


	getchar();
    return 0;
}

