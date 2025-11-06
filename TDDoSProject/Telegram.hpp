#pragma once
#include <tgbot/tgbot.h>
#include <set>

#define MAXCOMMANDLENGTH 100

class CTelegram
{
private:
	const std::int64_t m_dlBotStartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::set<int> m_hProcessedMessageIds;
	const char *m_pszTgBotToken = "";
	const long long m_dlChatId = 0;
	TgBot::Bot m_hTgBot;
	TgBot::TgLongPoll m_hLongPoll;

public:
	CTelegram();
	~CTelegram();

private:
	void Initialize();
	void UnInitialize();

	bool CommandParse(TgBot::Message::Ptr hMessage);
	void TSendMessage(const char *pszFormat, ...);
	void TSendDownload(std::string strPath);
	void TSendUpload(std::string strPath);

	bool CheckChatID(long long dlChatID);

public:
	void BeginBot();
};