// TestReseau1.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

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

int main()
{
	printf("Configuration du socket\n");
	int handle = socket(AF_INET,
						SOCK_DGRAM,
						IPPROTO_UDP);
	
	printf("Test du socket : ");
	if (handle <= 0)
	{
		printf("échec de création du socket!\n");
		return false;
	}
	printf("réussi!");

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( (unsigned short) 3000 );

	if (bind(handle,
		(const sockaddr*)&address,
		sizeof(sockaddr_in)) < 0) 
	{
		printf("échec du bind socket!\n");
		return false;
	}


	DWORD nonBlocking = 1;
	if (ioctlsocket(handle,
		FIONBIO,
		&nonBlocking) != 0)
	{
		printf("échec du parametrage non-blocking\n");
		return false;
	}



	getchar();
    return 0;
}

