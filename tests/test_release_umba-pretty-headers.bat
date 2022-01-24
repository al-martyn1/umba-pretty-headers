@if exist ..\_libs\_pretty_headers\clear.bat call ..\_libs\_pretty_headers\clear.bat       > clear.log
@..\.out\msvc2019\x64\Release\umba-pretty-headers.exe @data\umba-pretty-headers-09.rsp --verbose=detailed
