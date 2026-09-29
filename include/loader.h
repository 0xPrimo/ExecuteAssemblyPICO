#pragma once

#include <windows.h>
#include "tcg.h"

WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc( LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect );
#define VirtualAlloc KERNEL32$VirtualAlloc

typedef int ( *DOTNET_EXECUTE_ASSEMBLY_FUNC )( char* assembly, int assembly_size, char* args, int args_size );

WINBASEAPI DWORD WINAPI  KERNEL32$GetLastError( VOID );
WINBASEAPI HANDLE WINAPI KERNEL32$CreateNamedPipeA(
    LPCSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode, DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES lpSecurityAttributes );
WINBASEAPI WINBOOL WINAPI KERNEL32$ConnectNamedPipe( HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped );
WINBASEAPI HWND APIENTRY  KERNEL32$GetConsoleWindow( VOID );
WINBASEAPI WINBOOL WINAPI KERNEL32$AllocConsole( VOID );
WINBASEAPI HANDLE WINAPI  KERNEL32$GetStdHandle( DWORD nStdHandle );
WINBASEAPI WINBOOL WINAPI KERNEL32$SetStdHandle( DWORD nStdHandle, HANDLE hHandle );
WINUSERAPI WINBOOL WINAPI USER32$ShowWindow( HWND hWnd, int nCmdShow );

#define CreateNamedPipeA KERNEL32$CreateNamedPipeA
#define GetLastError KERNEL32$GetLastError
#define ConnectNamedPipe KERNEL32$ConnectNamedPipe
#define GetConsoleWindow KERNEL32$GetConsoleWindow
#define AllocConsole KERNEL32$AllocConsole
#define ShowWindow USER32$ShowWindow
#define GetStdHandle KERNEL32$GetStdHandle
#define SetStdHandle KERNEL32$SetStdHandle

#ifdef _DEBUG
#define LOG_ERROR( x, ... ) dprintf( "[-] " x "", ##__VA_ARGS__ )
#define LOG_INFO( x, ... ) dprintf( "[*] " x "", ##__VA_ARGS__ )
#define LOG_SUCCESS( x, ... ) dprintf( "[+] " x "", ##__VA_ARGS__ )
#else
#define LOG_ERROR( x, ... )
#define LOG_INFO( x, ... )
#define LOG_SUCCESS( x, ... )
#endif