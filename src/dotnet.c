#include "dotnet/dotnet.h"

GUID C_MH = { 0x9280188d, 0xe8e, 0x4867, { 0xb3, 0xc, 0x7f, 0xa8, 0x38, 0x84, 0xe8, 0xde } };
GUID I_MH = { 0xD332DB9E, 0xB9B3, 0x4125, { 0x82, 0x07, 0xA1, 0x48, 0x84, 0xF5, 0x32, 0x16 } };
GUID C_RH = { 0xcb2f6723, 0xab3a, 0x11d2, { 0x9c, 0x40, 0x00, 0xc0, 0x4f, 0xa3, 0x0a, 0x3e } };
GUID I_RH = { 0xcb2f6722, 0xab3a, 0x11d2, { 0x9c, 0x40, 0x00, 0xc0, 0x4f, 0xa3, 0x0a, 0x3e } };
GUID I_AD = { 0x05F696DC, 0x2B29, 0x3663, { 0xAD, 0x8B, 0xC4, 0x38, 0x9C, 0xF2, 0xA7, 0x13 } };
GUID I_RI = { 0xBD39D1D2, 0xBA2F, 0x486a, { 0x89, 0xB0, 0xB4, 0xB0, 0xCB, 0x46, 0x68, 0x91 } };

int ExecuteAssembly( char* assembly, int assembly_size, char* args, int args_size ) {
    CLR_CONTEXT ctx            = { 0 };
    HRESULT     result         = S_OK;
    wchar_t*    appdomain_name = L"1337";
    int         status         = FALSE;
    VARIANT     var_null;
    VARIANT     var_result;

    if (!ClrStart( &ctx, assembly, assembly_size )) {
        LOG_ERROR( "Failed to start CLR" );
        return status;
    }

    result = ctx.RuntimeHost->lpVtbl->CreateDomain( ctx.RuntimeHost, appdomain_name, NULL, &ctx.AppDomainUnk );
    if (result != S_OK) {
        if (result == E_OUTOFMEMORY) {
            LOG_ERROR( "Not enough memory in target process, please select another" );
        } else {
            LOG_ERROR( "Error CreateDomain : %lx", result );
        }

        goto cleanup;
    }

    result = ctx.AppDomainUnk->lpVtbl->QueryInterface( ctx.AppDomainUnk, &I_AD, (void**)&ctx.AppDomain );
    if (result != S_OK) {
        LOG_ERROR( "Error QueryInterface 2 : %lx", result );
        goto cleanup;
    }

    if (!LoadAssembly( &ctx, assembly, assembly_size )) {
        return FALSE;
    }

    result = ctx.Assembly->lpVtbl->get_EntryPoint( ctx.Assembly, (_MethodInfo**)&ctx.Entry );
    if (result != S_OK) {
        LOG_ERROR( "Error get_EntryPoint : %lx", result );
        goto cleanup;
    }

    if (!SetupEntryParams( &ctx, args )) {
        return FALSE;
    }

    // Invoke assembly entrypoint
    var_null.vt    = VT_NULL;
    var_null.plVal = NULL;
    result         = ctx.Entry->lpVtbl->Invoke_3( ctx.Entry, var_null, ctx.Params, &var_result );
    if (result != S_OK) {
        LOG_ERROR( "[-] .NET Runtime Error: %lx", result );
        goto cleanup;
    }

    status = TRUE;

    LOG_SUCCESS( "Execution complete" );

cleanup:
    ClrCleanup( &ctx );

    return status;
}

int GetDotnetVersion( char* assembly, int size ) {
    CHAR dotnet_v4_sig[] = "\x76\x34\x2E\x30\x2E\x33\x30\x33\x31\x39";

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 10; j++) {
            if (dotnet_v4_sig[j] != assembly[i + j])
                break;
            else if (j == 9)
                return DOTNET_V_4_0;
        }
    }

    RtlSecureZeroMemory( dotnet_v4_sig, sizeof( dotnet_v4_sig ) );

    return DOTNET_V_2_0;
}

int ClrStart( CLR_CONTEXT* ctx, char* assembly, int size ) {
    HRESULT result     = S_OK;
    BOOL    loadable   = FALSE;
    WCHAR   dotnetV4[] = L"v4.0.30319";
    WCHAR   dotnetV2[] = L"v2.0.50727";
    DWORD   dotnet_ver = GetDotnetVersion( assembly, size );

    result = CLRCreateInstance( &C_MH, &I_MH, (LPVOID*)&ctx->MetaHost );
    if (result != S_OK) {
        LOG_ERROR( "Error CLRCreateInstance : %lx", result );
        return FALSE;
    }

    result = ctx->MetaHost->lpVtbl->GetRuntime( ctx->MetaHost, ( dotnet_ver == DOTNET_V_4_0 ) ? dotnetV4 : dotnetV2, &I_RI, (PVOID*)&ctx->RuntimeInfo );
    if (result != S_OK) {
        return FALSE;
    }

    result = ctx->RuntimeInfo->lpVtbl->IsLoadable( ctx->RuntimeInfo, &loadable );
    if (result != S_OK || !loadable) {
        return FALSE;
    }

    result = ctx->RuntimeInfo->lpVtbl->GetInterface( ctx->RuntimeInfo, &C_RH, &I_RH, (PVOID*)&ctx->RuntimeHost );
    if (result != S_OK) {
        LOG_ERROR( "Incompatible .NET Framework version" );
        return FALSE;
    }

    result = ctx->RuntimeHost->lpVtbl->Start( ctx->RuntimeHost );
    if (result != S_OK) {
        LOG_ERROR( "Error starting CLR : %lx", result );
        return FALSE;
    }

    return TRUE;
}

void ClrCleanup( CLR_CONTEXT* ctx ) {

    SafeArrayDestroy( ctx->Params );

    if (ctx->Entry) {
        ctx->Entry->lpVtbl->Release( ctx->Entry );
    }
    if (ctx->Assembly) {
        ctx->Assembly->lpVtbl->Release( ctx->Assembly );
    }
    if (ctx->AppDomainUnk) {
        ctx->AppDomainUnk->lpVtbl->Release( ctx->AppDomainUnk );
    }
    if (ctx->AppDomain) {
        ctx->AppDomain->lpVtbl->Release( ctx->AppDomain );
    }
    if (ctx->RuntimeHost) {
        ctx->RuntimeHost->lpVtbl->UnloadDomain( ctx->RuntimeHost, (IUnknown*)ctx->AppDomain );
    }
    if (ctx->RuntimeInfo) {
        ctx->RuntimeInfo->lpVtbl->Release( ctx->RuntimeInfo );
    }
    if (ctx->MetaHost) {
        ctx->MetaHost->lpVtbl->Release( ctx->MetaHost );
    }
}

int LoadAssembly( CLR_CONTEXT* ctx, char* assembly, int size ) {
    HRESULT        result = S_OK;
    SAFEARRAY*     data;
    SAFEARRAYBOUND SafeArrayBound[1];
    PVOID          Buffer;

    SafeArrayBound[0].cElements = size;
    SafeArrayBound[0].lLbound   = 0;
    data                        = (SAFEARRAY*)SafeArrayCreate( VT_UI1, 1, SafeArrayBound );

    SafeArrayAccessData( data, &Buffer );
    CopyMemoryEx( Buffer, assembly, size );
    SafeArrayUnaccessData( data );

    result = ctx->AppDomain->lpVtbl->Load_3( ctx->AppDomain, data, (_Assembly**)&ctx->Assembly );
    if (result != S_OK) {
        LOG_ERROR( "Error Load_3 : %lx", result );
        return FALSE;
    }

    return TRUE;
}

int SetupEntryParams( CLR_CONTEXT* ctx, char* cmdline ) {
    HRESULT        result = S_OK;
    long           i      = 0;
    SAFEARRAY*     params;
    long           lbound_index;
    long           ubound_index;
    ULONG          elements;
    VARIANT        var_args;
    SAFEARRAYBOUND array_bound[1];
    int            args;
    wchar_t        cmdlineW[256];
    wchar_t**      cmdline_list = NULL;

    result = ctx->Entry->lpVtbl->GetParameters( ctx->Entry, &params );
    if (result != S_OK) {
        LOG_ERROR( "Error GetParameters : %lx", result );
        return FALSE;
    }

    result = SafeArrayGetLBound( params, 1, &lbound_index );
    if (result != S_OK) {
        LOG_ERROR( "Error SafeArrayGetLBound : %lx", result );
        return FALSE;
    }

    result = SafeArrayGetUBound( params, 1, &ubound_index );
    if (result != S_OK) {
        LOG_ERROR( "Error SafeArrayGetUBound : %lx", result );
        return FALSE;
    }

    elements = ubound_index - lbound_index + 1;
    if (elements != 0) {
        ctx->Params = SafeArrayCreateVector( VT_VARIANT, 0, 1 );
        if (cmdline) {
            MultiByteToWideChar( CP_ACP, 0, cmdline, -1, cmdlineW, 256 );
            cmdline_list    = CommandLineToArgvW( cmdlineW, &args );
            var_args.vt     = ( VT_ARRAY | VT_BSTR );
            var_args.parray = SafeArrayCreateVector( VT_BSTR, 0, args );

            for (i = 0; i < args; i++) {
                SafeArrayPutElement( var_args.parray, &i, SysAllocString( cmdline_list[i] ) );
            }
        } else {
            var_args.vt              = ( VT_ARRAY | VT_BSTR );
            array_bound[0].lLbound   = 0;
            array_bound[0].cElements = 0;
            var_args.parray          = SafeArrayCreate( VT_BSTR, 1, array_bound );
            array_bound[0].cElements = 1;
            ctx->Params              = SafeArrayCreate( VT_VARIANT, 1, array_bound );
        }

        i = 0;
        SafeArrayPutElement( ctx->Params, &i, &var_args );
        SafeArrayDestroy( var_args.parray );
    } else {
        ctx->Params = SafeArrayCreateVector( VT_EMPTY, 0, 0 );
    }

    return TRUE;
}

PVOID CopyMemoryEx( _Inout_ PVOID Destination, _In_ CONST PVOID Source, _In_ SIZE_T Length ) {
    PBYTE D = (PBYTE)Destination;
    PBYTE S = (PBYTE)Source;

    while (Length--)
        *D++ = *S++;

    return Destination;
}
