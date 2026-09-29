x64:
    load "bin/dotnet.x64.o"
        make object +disco

	mergelib "./libs/libtcg.x64.zip"

    exportfunc "ExecuteAssembly"  "__tag_dotnet_execute_assembly"

    export