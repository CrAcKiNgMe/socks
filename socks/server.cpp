#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define LogFileSwitchOn 1


#ifdef _MSC_VER
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <stdio.h>
#include <string.h>

#define Log printf
BOOL  InitWinSock();


class Server
{

	SOCKET m_socket;



};


int main()
{
	struct	sockaddr_in		sa_server;
	struct	sockaddr_in		sa_client;


	InitWinSock();

	SOCKET m_socket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP );

	if(m_socket  == INVALID_SOCKET)
	{
		printf("could not create socket %d", WSAGetLastError());
		return 2;
	}

	sa_server.sin_family			= AF_INET;
	sa_server.sin_addr.s_addr		= inet_addr( "127.0.0.1" );
	sa_server.sin_port				= htons(10088);


	if( bind(m_socket ,(struct sockaddr *)&sa_server , sizeof(sa_server)) == SOCKET_ERROR)
	{
		Log("Bind failed with error code : %d" , WSAGetLastError());
		closesocket(m_socket);
		return 3;
	}


	int c = sizeof(sockaddr_in);
	
	 listen(m_socket , SOMAXCONN);

	 int nsize = sizeof(sa_client);

	 printf("waiting for connection\n");

	 SOCKET accept_socket = accept(m_socket, (sockaddr*)&sa_client, &nsize);

	 if (accept_socket == INVALID_SOCKET) {
		 printf("accept failed: %d\n", WSAGetLastError());
		 closesocket(m_socket);
		 WSACleanup();
		 return 1;
	 }
	 else 
	 {
		 printf("Client connected.\n");
	 
		 if (SOCKET_ERROR == ioctlsocket(accept_socket, FIONBIO, &cmd))
		 {
			 Log("ioctlsocket error %d", WSAGetLastError());
			 //ErrorProcess();
			 closesocket(m_AcceptedSocket);
			 return 2;
		 }


	 }


	 // No longer need server socket
	 closesocket(m_socket);
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

