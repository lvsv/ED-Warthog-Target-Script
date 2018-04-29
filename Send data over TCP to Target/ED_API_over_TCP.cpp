// ED_API_over_TCP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <Winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	char buf[128];
	sockaddr_in addr;

	WSADATA WSAData;
	WSAStartup(2, &WSAData);
	gethostname(buf, sizeof(buf));
	hostent *server = gethostbyname(buf);
	if (server)
	{
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s != INVALID_SOCKET)
		{
			memcpy(&addr.sin_addr, server->h_addr, server->h_length);
			addr.sin_family = server->h_addrtype;
			addr.sin_port = htons(1000); // use TCP port 1000, the same as the one in TARGET script
			if (!connect(s, (sockaddr*)&addr, sizeof(addr)))
			{
				*(unsigned short*)buf = 2 + 19;		// data size = 12 bytes (packet length + data)
				strcpy(buf + 2, "Send to TARGET TEST");		// 10 bytes of data (can be anything)
				send(s, buf, *(unsigned short*)buf, 0);	// send (packet length + data) bytes of data
			}
			closesocket(s);
		}
	}
	WSACleanup();
}

