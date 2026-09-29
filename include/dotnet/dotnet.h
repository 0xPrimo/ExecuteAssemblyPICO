#pragma once

#include <windows.h>
#include "dotnet/metahost.h"
#include "dotnet/mscorlib.h"
#include "tcg.h"

WINBASEAPI HRESULT WINAPI MSCOREE$CLRCreateInstance( REFCLSID clsid, REFIID riid, LPVOID* ppInterface );
WINBASEAPI LPWSTR*        SHELL32$CommandLineToArgvW( LPCWSTR lpCmdLine, int* pNumArgs );
WINBASEAPI SAFEARRAY*     OLEAUT32$SafeArrayCreate( VARTYPE vt, UINT cDims, SAFEARRAYBOUND* rgsabound );
WINBASEAPI HRESULT        OLEAUT32$SafeArrayDestroy( SAFEARRAY* psa );
WINBASEAPI HRESULT        OLEAUT32$SafeArrayGetLBound( SAFEARRAY* psa, UINT nDim, LONG* plLbound );
WINBASEAPI HRESULT        OLEAUT32$SafeArrayGetUBound( SAFEARRAY* psa, UINT nDim, LONG* plLbound );
WINBASEAPI BSTR           OLEAUT32$SysAllocString( const OLECHAR* psz );
WINBASEAPI HRESULT        OLEAUT32$SafeArrayPutElement( SAFEARRAY* psa, LONG* rgIndices, void* pv );
WINBASEAPI SAFEARRAY*     OLEAUT32$SafeArrayCreateVector( VARTYPE vt, LONG lLbound, ULONG cElements );
WINBASEAPI HRESULT        OLEAUT32$SafeArrayUnaccessData( SAFEARRAY* psa );
WINBASEAPI HRESULT        OLEAUT32$SafeArrayAccessData( SAFEARRAY* psa, void HUGEP** ppvData );
WINBASEAPI int WINAPI     KERNEL32$MultiByteToWideChar( UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar );

#define CLRCreateInstance MSCOREE$CLRCreateInstance
#define CommandLineToArgvW SHELL32$CommandLineToArgvW
#define SafeArrayCreate OLEAUT32$SafeArrayCreate
#define SafeArrayDestroy OLEAUT32$SafeArrayDestroy
#define SafeArrayGetLBound OLEAUT32$SafeArrayGetLBound
#define SafeArrayGetUBound OLEAUT32$SafeArrayGetUBound
#define SysAllocString OLEAUT32$SysAllocString
#define SafeArrayPutElement OLEAUT32$SafeArrayPutElement
#define SafeArrayCreateVector OLEAUT32$SafeArrayCreateVector
#define SafeArrayUnaccessData OLEAUT32$SafeArrayUnaccessData
#define SafeArrayAccessData OLEAUT32$SafeArrayAccessData
#define MultiByteToWideChar KERNEL32$MultiByteToWideChar

typedef struct {
    ICLRMetaHost*    MetaHost;
    ICLRRuntimeInfo* RuntimeInfo;
    ICorRuntimeHost* RuntimeHost;
    IUnknown*        AppDomainUnk;
    _AppDomain*      AppDomain;
    _Assembly*       Assembly;
    _MethodInfo*     Entry;
    SAFEARRAY*       Params;
} CLR_CONTEXT, *PCLR_CONTEXT;

#ifdef _DEBUG
#define LOG_ERROR( x, ... ) dprintf( "[-] " x "", ##__VA_ARGS__ )
#define LOG_INFO( x, ... ) dprintf( "[*] " x "", ##__VA_ARGS__ )
#define LOG_SUCCESS( x, ... ) dprintf( "[+] " x "", ##__VA_ARGS__ )
#else
#define LOG_ERROR( x, ... )
#define LOG_INFO( x, ... )
#define LOG_SUCCESS( x, ... )
#endif

#define DOTNET_V_2_0 1
#define DOTNET_V_4_0 2

int   GetDotnetVersion( char* assembly, int size );
int   ClrStart( CLR_CONTEXT* ctx, char* assembly, int size );
void  ClrCleanup( CLR_CONTEXT* ctx );
int   LoadAssembly( CLR_CONTEXT* ctx, char* assembly, int size );
int   SetupEntryParams( CLR_CONTEXT* ctx, char* cmdline );
PVOID CopyMemoryEx( _Inout_ PVOID Destination, _In_ CONST PVOID Source, _In_ SIZE_T Length );
