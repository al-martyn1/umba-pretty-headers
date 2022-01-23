@rem %UMBA_TOOLS% - eg F:\umba-tools

@if not exist %UMBA_TOOLS%\bin    mkdir %UMBA_TOOLS%\bin
@if not exist %UMBA_TOOLS%\conf   mkdir %UMBA_TOOLS%\conf

@copy /Y .out\msvc2019\x64\Release\umba-make-headers.exe     %UMBA_TOOLS%\bin\
@copy /Y .out\msvc2019\x64\Release\umba-pretty-headers.exe   %UMBA_TOOLS%\bin\

@xcopy /Y /S /E /I /F /R _distr_conf\conf\*                  %UMBA_TOOLS%\conf

set CLANG_INCLUDE=G:\llvm-built\msvc2019\x64\Release\lib\clang\13.0.1\include
@if not exist %CLANG_INCLUDE% goto NO_CLANG_ICLUDES
@xcopy /Y /S /E /I /F /R %CLANG_INCLUDE%\* %UMBA_TOOLS%\lib\clang\13.0.1\include

:NO_CLANG_ICLUDES

