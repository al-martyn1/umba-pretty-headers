@set UPH=.out\msvc2019\x64\Debug\umba-pretty-headers.exe
@if exist _libs\_pretty_headers\clear.bat call _libs\_pretty_headers\clear.bat
@rem set CFG_TEST_MODE=--test-config
@set CFG_TEST_MODE=
@set DOJOBALWAYS=--ignore-parsing-errors
@rem set DOJOBALWAYS=
%UPH% @umba-pretty-headers.rsp %CFG_TEST_MODE% %DOJOBALWAYS%