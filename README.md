# ExecuteAssemblyPICO

## About

A Position Independent Code (PIC) implementation of the execute-assembly command, using Crysta Palace linker.

## How it Works?

- The loader creates a named pipe and sets it as the current process's stdout to capture the executed .NET assembly output.
- It then loads the PICO ".NET assembly loader".
- Finally, it passes the .NET assembly and its arguments to the PICO's exported function "ExecuteAssembly".

## Build

- You will need first to install the crystal palace linker by follow this [documentation](https://tradecraftgarden.org/docs.html).

```bash
# release
make x64 ASSEMBLY_PATH="/path/to/Rubeus.exe" ASSEMBLY_ARGS="triage" PIPE_NAME="\\\\.\\pipe\\abc" 

# debug 
make x64-debug ASSEMBLY_PATH="/path/to/Rubeus.exe" ASSEMBLY_ARGS="triage"
```

## Credits

- [@kyleavery: inject-assembly](https://github.com/kyleavery/inject-assembly)
- [@ofasgard: execute-assembly-pico](https://github.com/ofasgard/execute-assembly-pico)
- [TradeCraft Garden](https://tradecraftgarden.org/docs.html)