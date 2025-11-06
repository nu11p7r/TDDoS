#include "Zip.hpp"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/copy.hpp>

namespace io = boost::iostreams;
namespace fs = boost::filesystem;

CZip::CZip()
{
}

CZip::~CZip()
{
}

void CZip::Initialize()
{
}

void CZip::UnInitialize()
{
}

bool CZip::IsDirectory(const std::string &pPath)
{
	struct stat hInfo;

	if (stat(pPath.c_str(), &hInfo) != 0)
	{
		return false;
	}

	return (hInfo.st_mode & S_IFDIR) != 0;
}

bool CZip::IsRegularFile(const std::string &pPath)
{
	struct stat hInfo;

	if (stat(pPath.c_str(), &hInfo) != 0)
	{
		return false;
	}

	return (hInfo.st_mode & S_IFREG) != 0;
}

void CZip::AddFileToZip(const std::string &zipFilename, const std::string &filename)
{
    try 
    {
        // Open the zip file for appending
        io::filtering_streambuf<io::output> out;
        out.push(io::zlib_compressor());
        std::ofstream file(zipFilename, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
        out.push(file);

        // Open the file to add to the zip
        std::ifstream in(filename, std::ios_base::binary);

        if (!in) 
        {
            //std::cerr << "Cannot open input file: " << filename << std::endl;
            return;
        }

        // Copy the file into the zip
        io::copy(in, out);
        in.close();
    }
    catch (const std::exception &ex) 
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
}
void CZip::listFiles(const std::string &directoryPath)
{
    try {
        fs::path dirPath(directoryPath);

        fs::recursive_directory_iterator hDirectoryIterator(dirPath);

        for (; hDirectoryIterator != fs::recursive_directory_iterator(); hDirectoryIterator++)
        {
            if (is_regular_file(hDirectoryIterator->status()))
            {
                AddFileToZip("Test.zip", hDirectoryIterator->path().filename().string());
            }
        }
    }
    catch (const std::exception &ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
}
