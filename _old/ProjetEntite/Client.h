#pragma once

#define BUF_SIZE	1024

typedef struct
{
	SOCKET sock;
	char name[BUF_SIZE];
}Client;