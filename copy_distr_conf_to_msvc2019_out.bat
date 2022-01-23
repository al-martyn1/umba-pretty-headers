@xcopy /Y /S /E /I /F /R _distr_conf\conf\* .out\msvc2019\x64\conf
@xcopy /Y /S /E /I /F /R _distr_conf\conf\* .out\msvc2019\x86\conf

set CLANG_INCLUDE=G:\llvm-built\msvc2019\x64\Release\lib\clang\13.0.1\include
@if not exist %CLANG_INCLUDE% goto NO_CLANG_ICLUDES
@xcopy /Y /S /E /I /F /R %CLANG_INCLUDE%\* .out\msvc2019\x86\lib\clang\13.0.1\include
@xcopy /Y /S /E /I /F /R %CLANG_INCLUDE%\* .out\msvc2019\x64\lib\clang\13.0.1\include

:NO_CLANG_ICLUDES
