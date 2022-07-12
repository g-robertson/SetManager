# Build Requirements:
## Windows
- Executables needed for standard building `cmake` `make` `g++`: 
## Linux
### Arch Linux
- Packages needed for standard building: `base-devel`
- Packages needed for Linux for Windows building: `mingw-w64-gcc`

# Build Steps
1. Create a build directory
2. Change directory into the build directory
3. Run the commands `cmake ..` (include flags if you want them) then `make`
## Linux for Windows building
You can build a Windows executable on a Linux machine for testing by including the flag `-DLINWIN32=TRUE` or `-DLINWIN64=TRUE` with cmake