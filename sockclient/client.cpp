#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define LogFileSwitchOn 1

#ifdef _MSC_VER
#include <Windows.h>

#include <WinSock2.h>
#include <varargs.h>
#pragma comment(lib, "Ws2_32.lib")
#endif
#include <stdio.h>
#include <string.h>


void Log(const wchar_t* format, ... );
void Log(const char* format, ... );
int  LogFile(const char* format,  ...);
BOOL  InitWinSock();


int main()
{
	InitWinSock();

	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	clientService.sin_port = htons( 10088 );

	//----------------------
	// Connect to server.
	if ( connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
		printf( "Failed to connect.\n" );
		WSACleanup();
		return 0;
	}






	printf("Connected to server.\n");
	WSACleanup();
	return 0;

}


BOOL  InitWinSock()
{


	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		Log("Tell the user that we could not find a usable WinSock DLL");
		return 1;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		Log("Tell the user that we could not find a usable WinSock DLL");  
		WSACleanup();
		return 1; 
	}



	Log("init socket success");


	return 0;
}


void Log(const wchar_t* format, ... )
{
	WCHAR Buf[0x2048] = {0};


	va_list vList;
	va_start(vList, format);


	_vsnwprintf(Buf, 0x2047,format,vList);

	va_end(vList);


	OutputDebugStringW(Buf);

}


void Log(const char* format, ... )
{
	CHAR Buf[0x2048] = {0};


	va_list vList;
	va_start(vList, format);


	_vsnprintf(Buf,0x2047, format,vList);

	va_end(vList);


	OutputDebugStringA(Buf);

}


int LogFile(const char* format,  ...)
{
#if LogFileSwitchOn





	CHAR Buf[0x4000] = {0};
	va_list vList;

	va_start(vList, format); 
	vsprintf(Buf, format, vList);
	va_end(vList);



	char szAppPathName[256] = {0};
	char szLogFileName[256] = {0};


	//GetModuleFileNameA(NULL,szLogFileName, 256 );


	char szAppDataDir[256] = {0};


	::ExpandEnvironmentStringsA("%APPDATA%\\GeeMee", szAppDataDir, MAX_PATH);


	if(!PathFileExistsA(szAppDataDir) )
	{
		CreateDirectoryA(szAppDataDir, NULL);

	}



	PathAppendA(szAppDataDir, PGLogFileName);

	//	Log("LogFileName %s", szAppDataDir);




	HANDLE hLogFile = CreateFileA(szAppDataDir,\
		GENERIC_WRITE,\
		FILE_SHARE_WRITE,\
		NULL,\
		OPEN_ALWAYS,\
		NULL,\
		NULL);

	if(hLogFile == INVALID_HANDLE_VALUE)
	{
		hLogFile = NULL;
		Log("Can not open log file %d", GetLastError());
	}

	unsigned long uDistanceToMoveHigh = 0;
	SetFilePointer(hLogFile,NULL, NULL, FILE_END);


	DWORD dwBytesWritten = 0;


	SYSTEMTIME st;
	GetSystemTime(&st);
	SYSTEMTIME localTime;
	memset(&localTime, 0, sizeof(SYSTEMTIME));

	TIME_ZONE_INFORMATION zinfo;
	GetTimeZoneInformation(&zinfo);
	SystemTimeToTzSpecificLocalTime(&zinfo,&st, &localTime);

	char systime[256] = {0};
	sprintf(systime, "%04d%02d%02d-%02d:%02d:%02d.%-06d", localTime.wYear,localTime.wMonth,localTime.wDay, localTime.wHour, localTime.wMinute,localTime.wSecond,localTime.wMilliseconds );
	char threadid[256] = {0};
	sprintf(threadid, "%-06d", GetCurrentThreadId());

	WriteFile(hLogFile, systime, strlen(systime), &dwBytesWritten, NULL);
	WriteFile(hLogFile, threadid, strlen(threadid), &dwBytesWritten, NULL);
	WriteFile(hLogFile, Buf, strlen(Buf), &dwBytesWritten, NULL);
	WriteFile(hLogFile, "\r\n", strlen("\r\n"), &dwBytesWritten, NULL);

	CloseHandle(hLogFile);

#endif


	return 0;
}
