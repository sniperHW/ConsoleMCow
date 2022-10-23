//#include "pch.h"
#include "CWdebug.h"
#include "util.h"
#include <fstream>

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

