//#include "pch.h"
#include "CWdebug.h"
#include "util.h"
#include <fstream>
#include <io.h>

using namespace std;


void CWdebug::Log(const string& sLog)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\Log\\";
	string sFilePath = sConfigFolder + "Log.txt";

	ofstream fout(sFilePath, ios_base::out | ios_base::app);

	if (fout.is_open()) 
		fout << getTimeString() << "\t" << sLog << endl;

	fout.close();
}

void CWdebug::DeleteDump()
{

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sFolder = buffer;
	sFolder = sFolder + "\\dump";

	intptr_t hFile;
	struct _finddata_t fileinfo;
	string path = sFolder;
	string sFilePath;


	string p;
	hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo);
	while (_findnext(hFile, &fileinfo) == 0)
	{
		if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0 || fileinfo.attrib == _A_SUBDIR)
			continue;

		sFilePath = path + "\\" + string(fileinfo.name);
		cout << sFilePath << endl;
		remove(sFilePath.c_str());
	}
}
