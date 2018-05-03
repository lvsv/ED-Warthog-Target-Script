#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <Winsock2.h>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <time.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>

using namespace rapidjson;

#pragma comment(lib, "ws2_32.lib")

int main()
{
	std::cout << "Hello world!" << std::endl;
	std::cout << "ED-Warthog-Target-Script 3.x : Status.json Flags Addon" << std::endl;

	for (;;) {

		//Load the Stuatus.json file
//		std::ifstream ifs("C:/Users/tomdu/Saved Games/Frontier Developments/Elite Dangerous/Status.json");
		std::ifstream ifs("Status.json");

		if (!ifs.is_open()) {
			std::cerr << "Can't open Status.json.This program should be in : /Users/.../Saved Games/Frontier Developments/Elite Dangerous/" << std::endl;
			system("PAUSE");
			return 0;
		}

		IStreamWrapper isw { ifs };
		Document status{}; // create RapidJSON value
		status.ParseStream(isw);

		StringBuffer buffer{};
		Writer<StringBuffer> writer{ buffer };
		status.Accept(writer);

		ifs.close(); //Close the status.json file

		if (status.HasParseError())
		{
			std::cout << "Error  : " << status.GetParseError() << '\n'
			<< "Offset : " << status.GetErrorOffset() << '\n';
			return EXIT_FAILURE;
		}

		const std::string jsonStr{ buffer.GetString() };
		std::cout << jsonStr << '\n';

		//Retreive Json Flags Value
		int v_Flags = 0;
		if (status.HasMember("Flags")) {
			assert(status["Flags"].IsNumber()); //Flags
			v_Flags = status["Flags"].GetInt();
			//printf("Flags = %d\n", v_Flags);
		}
		
		//Retreive Json Pips Value
		if (status.HasMember("Pips")){
			const Value& p = status["Pips"];	//Pips as Array
			assert(p.IsArray());
			/*for (SizeType i = 0; i < p.Size(); i++) { // Uses SizeType instead of size_t
				printf("p[%d] = %d\n", i, p[i].GetInt());
			}*/
		}

		status.SetObject();  // clear RapidJson Document and minimize

		//If In Game do the stuff below
		if (v_Flags != 0) {

			// convert Flags int to String
			std::string s_Flags = std::to_string(v_Flags);
			const char * c = s_Flags.c_str();

			/*//Display Flags Value & Time In Console
			time_t tim;  //create variable of time_t
			time(&tim); //pass variable tim to time function
			std::cout << "Flags : " << c << " / " << ctime(&tim) << std::endl;*/

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
					addr.sin_port = htons(1000);					// use TCP port 1000, the same as the one in TARGET script
					if (!connect(s, (sockaddr*)&addr, sizeof(addr)))
					{
						*(unsigned short*)buf = 2 + (unsigned short)strlen(c);		// data size = xxx bytes (packet length + data)
						strcpy_s(buf + 2, *(unsigned short*)buf, c);					// c : 8 bytes of data
						send(s, buf, *(unsigned short*)buf, 0);						// send (packet length + data) bytes of data
					}
					closesocket(s);
				}
			}
			WSACleanup();
			v_Flags = 0;
		}
		else { //You are not In Game !!!!
			std::cerr << "Warning : You need to be logged in Elite Dangerous ..." << std::endl;
			//system("PAUSE");
			Sleep(4000);
		}
		Sleep(1500);
	}
	return 0;
}