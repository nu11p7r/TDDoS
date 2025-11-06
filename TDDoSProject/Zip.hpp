#pragma once
#include <string>

class CZip
{
public:
	CZip();
	~CZip();

	void Initialize();
	void UnInitialize();

public:
	bool IsDirectory(const std::string &pPath);
	bool IsRegularFile(const std::string &pPath);
	void AddFileToZip(const std::string &zipFilename, const std::string &filename);
	void listFiles(const std::string &directoryPath);
};

