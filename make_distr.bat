@SETLOCAL ENABLEDELAYEDEXPANSION

@call copy_distr_conf_to_msvc2019_out.bat


@set DISTR_ROOT=.distr
@if not exist %DISTR_ROOT% mkdir %DISTR_ROOT%

@set MAIN_EXE_NAME=umba-pretty-headers
@set BUILD_OUTPUT_ROOT=.out\msvc2019


:TRY_X64_DEBUG
@if not exist %BUILD_OUTPUT_ROOT%\x64\Debug\%MAIN_EXE_NAME%.exe goto TRY_X64_RELEASE
@call :MK_DISTR x64 Debug msvc2019_64


:TRY_X64_RELEASE
@if not exist %BUILD_OUTPUT_ROOT%\x64\Release\%MAIN_EXE_NAME%.exe goto TRY_X86_DEBUG
@call :MK_DISTR x64 Release msvc2019_64


:TRY_X86_DEBUG
@if not exist %BUILD_OUTPUT_ROOT%\x86\Debug\%MAIN_EXE_NAME%.exe goto TRY_X86_RELEASE
@call :MK_DISTR x86 Debug msvc2019

:TRY_X86_RELEASE
@if not exist %BUILD_OUTPUT_ROOT%\x86\Release\%MAIN_EXE_NAME%.exe goto END
@call :MK_DISTR x86 Release msvc2019

goto END


:MK_DISTR
@set PLATFORM=%1
@set CONFIGURATION=%2
@set QTSUBPATH=%3
@set WINDEPLOYQTRELOPT=%2
@call :LoCase WINDEPLOYQTRELOPT
@set LCCONFIGURATION=%CONFIGURATION%
@call :LoCase LCCONFIGURATION
@echo Make distr for %PLATFORM%-%CONFIGURATION%, Qt Config: %QTSUBPATH%, deploy opt - %WINDEPLOYQTRELOPT%

@if exist %DISTR_ROOT%\%PLATFORM% rd /S /Q %DISTR_ROOT%\%PLATFORM%
@set BUILD_OUTPUT=%BUILD_OUTPUT_ROOT%\%PLATFORM%\%CONFIGURATION%
@set TARGET_ROOT=%DISTR_ROOT%\%PLATFORM%\%CONFIGURATION%\umba-pretty-headers
@mkdir %TARGET_ROOT%\bin
@xcopy /Y /S /E /I /F /R _distr_conf\conf\* %TARGET_ROOT%\conf
@copy %BUILD_OUTPUT%\%MAIN_EXE_NAME%.exe %TARGET_ROOT%\bin\%MAIN_EXE_NAME%.exe
@set WINDEPLOYQT=%MSVC2019_QTROOT%\%QTSUBPATH%\bin\windeployqt.exe
@rem %WINDEPLOYQT% >windeployqt.txt
@rem %WINDEPLOYQT% --%WINDEPLOYQTRELOPT% --compiler-runtime %TARGET_ROOT%\bin\  > windeployqt-%PLATFORM%-%CONFIGURATION%.log 2>&1
@set ZIPDISTRNAME=umba-pretty-headers_%PLATFORM%_%LCCONFIGURATION%.zip
@echo Zip: %ZIPDISTRNAME%
@rem CALL :M1 param1

@exit /b

@rem https://www.robvanderwoude.com/battech_convertcase.php
:LoCase
@rem Subroutine to convert a variable VALUE to all lower case.
@rem  The argument for this subroutine is the variable NAME.
@SET %~1=!%~1:A=a!
@SET %~1=!%~1:B=b!
@SET %~1=!%~1:C=c!
@SET %~1=!%~1:D=d!
@SET %~1=!%~1:E=e!
@SET %~1=!%~1:F=f!
@SET %~1=!%~1:G=g!
@SET %~1=!%~1:H=h!
@SET %~1=!%~1:I=i!
@SET %~1=!%~1:J=j!
@SET %~1=!%~1:K=k!
@SET %~1=!%~1:L=l!
@SET %~1=!%~1:M=m!
@SET %~1=!%~1:N=n!
@SET %~1=!%~1:O=o!
@SET %~1=!%~1:P=p!
@SET %~1=!%~1:Q=q!
@SET %~1=!%~1:R=r!
@SET %~1=!%~1:S=s!
@SET %~1=!%~1:T=t!
@SET %~1=!%~1:U=u!
@SET %~1=!%~1:V=v!
@SET %~1=!%~1:W=w!
@SET %~1=!%~1:X=x!
@SET %~1=!%~1:Y=y!
@SET %~1=!%~1:Z=z!
@exit /b


:END
@exit /b
