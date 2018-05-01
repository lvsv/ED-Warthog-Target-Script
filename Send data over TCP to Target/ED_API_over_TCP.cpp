#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <Winsock2.h>
#include <fstream>
#include <iostream>
#include <string>
#include <json.hpp>
#include <windows.h>
#include <time.h>

using json = nlohmann::json;

#pragma comment(lib, "ws2_32.lib")

int main()
{
	std::cout << "Hello world!" << std::endl;
	std::cout << "Waiting for the launch of ED game" << std::endl;

	for (;;) {	// Read & Parse file, Send Data
	
		//Load the file
		json j;

		std::ifstream ifs("C:/Users/tomdu/Saved Games/Frontier Developments/Elite Dangerous/Status.json");
		//std::ifstream ifs("Status.json");

		if (!ifs.is_open()) {
			return false;
		}
		ifs >> j;
		ifs.close();

		// verify that "Flags" attribute exists in j
		if (j.find("Flags") != j.end()) {
			json Flags = j["Flags"];
		}

		//Get Flags Value
		//	int i_Flags = 16842765; // For manual entry Test only
		auto i_Flags = j["Flags"].get<int>();

		if (i_Flags != 0) { //If In Game do the stuff below

			// convert Flags int to String
			std::string s_Flags = std::to_string(i_Flags);
			const char * c = s_Flags.c_str();

			//Display In Console
			time_t tim;  //create variable of time_t
			time(&tim); //pass variable tim to time function
			std::cout << ctime(&tim) << "Flags : " << c << std::endl;

			// Send Data through Socket TCP
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
					addr.sin_port = htons(1000);				// use TCP port 1000, the same as the one in TARGET script
					if (!connect(s, (sockaddr*)&addr, sizeof(addr)))
					{
						*(unsigned short*)buf = 2 + 8;			// data size = 10 bytes (packet length + data)
						//strcpy(buf + 2, c);						// c : 8 bytes of data
						strcpy_s(buf + 2, 10, c);						// c : 8 bytes of data
						send(s, buf, *(unsigned short*)buf, 0);	// send (packet length + data) bytes of data

					}
					closesocket(s);
				}
			}
			WSACleanup();
		}
	Sleep(1000);
	}
return 0;
}

