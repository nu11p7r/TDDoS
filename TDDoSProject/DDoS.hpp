#pragma once

const int FLOODSIZE = 4096;

class CDDoS
{
private: // 공통
	std::string m_strTargetIP;
	int m_iPort;
	int m_nType; // 공격 타입
	long m_lTick; // 공격 시간

private: // TCP

private: // UDP

public:
	CDDoS(std::string strTargetIP, int iPort, int nType, long lTick);
	~CDDoS();

	void Initialize();
	void UnInitialize();

public:
	void Excution();


	// 기능
private:
	DWORD Resolve(char *pszIP);

	void TCPSpoofing();
	void UDPSpoofing();

	void TCPFlood();
	void UDPFlood();
};