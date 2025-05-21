@echo off

set PARAM=%1
set COMPILE_TYPE=%2
set PRJ_PATH=%3
set RET_EXCEL=%4
set RET_ROW=%5
set RET_COL=%6

set TEST_WORKSPACE=Thinker_HAL

echo 步骤1，%PARAM% 编译/下载/测试：--------------------------------------

::set RESULT_DIR="%WORKSPACE%_Test\Result\%BUILD_ID%_%BUILD_USER_ID%_%W_BRANCH_NAME:~7,50%_%GIT_COMMIT:~0,8%\%PARAM%\%COMPILE_TYPE%"
::set BAK_DIR="Z:\CITEST_SDK\%JOB_BASE_NAME%\%BUILD_ID%_%W_BRANCH_NAME:~7,50%_%GIT_COMMIT:~0,8%\%PARAM%\%COMPILE_TYPE%\"
set RESULT_DIR="%WORKSPACE%_Test\Result\%BUILD_ID%_%BUILD_USER_ID%_%W_BRANCH_NAME:~7,50%\%PARAM%\%COMPILE_TYPE%"
set BAK_DIR="Z:\CITEST_SDK\%JOB_BASE_NAME%\%BUILD_ID%_%W_BRANCH_NAME:~7,50%\%PARAM%\%COMPILE_TYPE%\"
echo.
echo  -------------- Print Test info --------------
echo RESULT_DIR     -- %RESULT_DIR%
echo TEST_DIR       -- %TEST_DIR%
echo DownloadTool   -- %DownloadTool%
echo PRJ_NAME       -- %PRJ_NAME%
echo UART_H_PORT    -- %UART_H_PORT%
echo UART_L_PORT    -- %UART_L_PORT%
echo UART_BURN_PORT -- %UART_BURN_PORT%
echo RELAY_PORT     -- %RELAY_PORT%
echo RTYPE          -- %RTYPE%
echo RCHAN          -- %RCHAN%
echo BCHAN          -- %BCHAN%
echo DEVICE_TYPE    -- %DEVICE_TYPE%
echo W_BRANCH_NAME  -- %W_BRANCH_NAME%


echo.
echo 步骤1.1，%PARAM% 编译：--------------------------------------

setlocal EnableDelayedExpansion

set root_path=%cd%
call set_env.bat %COMPILE_TYPE%
echo ****** set compile_path **********
set compile_path=%PRJ_PATH%
cd !compile_path!
echo ****** !compile_path! **********
echo,-----------Build [!PRJ_NAME!] start------------
call scons --board=%PRJ_NAME% -j8
if !errorlevel! neq 0 goto BUILD_FAIL
echo,-----------Build [!PRJ_NAME!] end------------


REM set PARAM_COUNT=0
REM 计算传入的参数数量
REM for %%I in (%*) do (
REM    set /A PARAM_COUNT+=1
REM )


:BUILD_PASS
echo #########        #          ######        ######
echo #       #       # #        #             #
echo #       #      #   #      #             #
echo #########     #     #     #             #
echo #            #########     ###### #      ########
echo #           #         #            #             #
echo #          #           #           #             #
echo #         #             #   #######      ########
SET BUILD_RESULT=0
mkdir %RESULT_DIR%
goto BUILD_END

:BUILD_FAIL
echo Build project[%PRJ_NAME%] fail.
echo #########        #         #########    #
echo #               # #            #        #
echo #              #   #           #        #
echo #########     #     #          #        #         
echo #            #########         #        #
echo #           #         #        #        #
echo #          #           #       #        #
echo #         #             #  ##########   ##########
SET BUILD_RESULT=1
goto BUILD_END

:BUILD_END
cd %root_path%
if %BUILD_RESULT% equ 0 (
    robocopy  %compile_path%\build_%PRJ_NAME%_hcpu %RESULT_DIR% *.bin *.hex  *.ini  /S /LEV:3
) else (
    set DOWNLOAD_RESULT=3
    set TEST_RESULT=3
    goto ERREND
)

echo 步骤1.2，%PARAM% 下载：--------------------------------------
cd %TEST_DIR%
set cnt=0
set DOWNLOAD_RESULT=0

:PREBURN
MsDelay.exe 2000
SwitchReset.exe --type %RTYPE% --port %RELAY_PORT% --baund 9600 --msdelay 1000 --rchan %RCHAN% --bchan %BCHAN% --blevel 1
if %errorlevel% EQU -2 goto PREBURN
if %errorlevel% EQU -1 (
    set DOWNLOAD_RESULT=1
    set TEST_RESULT=3
    goto ERREND
)

set /a cnt+=1
if %cnt% gtr 2  (
    SET DOWNLOAD_RESULT=1
    set TEST_RESULT=3
    goto ERREND
)
%DownloadTool%ImgDownUart.exe --port %UART_BURN_PORT% --baund 3000000 --func 0 --loadram 1 --postact 1 --device %DEVICE_TYPE% --file %RESULT_DIR%\ImgBurnList.ini --log %RESULT_DIR%\ImgBurn.log
if %errorlevel% neq 0 goto PREBURN

:POSTBURN
MsDelay.exe 2000
SwitchReset.exe --type %RTYPE% --port %RELAY_PORT% --baund 9600 --msdelay 1000 --rchan %RCHAN% --bchan %BCHAN% --blevel 0
::SwitchReset.exe --type %RTYPE% --msdelay 1000 --rchan %RCHAN% --bchan %BCHAN% --blevel 0

if %errorlevel% EQU -2 goto POSTBURN
if %errorlevel% EQU -1 (
    SET DOWNLOAD_RESULT=1
    set TEST_RESULT=3
    goto ERREND
)

echo 步骤1.3，%PARAM% 测试：--------------------------------------

set TEST_RESULT=0
cd %TEST_DIR%\%TEST_WORKSPACE%
::call Thinker.exe --exit 1 --hardver %PRJ_NAME%  --softver %GIT_COMMIT:~0,8% --logpath %RESULT_DIR%\log
call Thinker.exe --exit 1 --dutport %UART_H_PORT% --rtdport %UART_L_PORT% --dutport2 %UART_L_PORT% --noreset --switchtype %RTYPE% --switchport %RELAY_PORT%  --dutrchan %RCHAN% --retexcel %RET_EXCEL% %RET_ROW% %RET_COL% --logpath %RESULT_DIR%\log
if %errorlevel% NEQ 0 (
    SET TEST_RESULT=1
    goto ERREND
)

:ERREND
copy /Y CaseInfo.xlsx  %RESULT_DIR%\CaseInfo.xlsx 
xcopy %RESULT_DIR% %BAK_DIR% /e /y /q
SwitchReset.exe --type %RTYPE% --port %RELAY_PORT% --baund 9600 --msdelay 1000 --rchan %RCHAN% --bchan %BCHAN% --blevel 0
cd  %WORKSPACE%
echo.
set /a all_result=%BUILD_RESULT%+%DOWNLOAD_RESULT%+%TEST_RESULT%

if %all_result%==9 set STEP_RESULT=2
if %all_result% LSS 9  set STEP_RESULT=1
if %all_result%==0 set STEP_RESULT=0

echo.
echo  -------------- %PARAM% STEP RESULT --------------
echo %PARAM%_BUILD_RESULT -- %BUILD_RESULT%
echo %PARAM%_DOWNLOAD_RESULT -- %DOWNLOAD_RESULT%
echo %PARAM%_TEST_RESULT -- %TEST_RESULT%
echo %PARAM%_STEP_RESULT -- %STEP_RESULT%
echo  -------------- %PARAM% STEP RESULT --------------
echo.
echo  -------------- %PARAM% STEP RESULT -------------- >> properties.properties
echo %PARAM%_BUILD_RESULT=%BUILD_RESULT% >> properties.properties
echo %PARAM%_DOWNLOAD_RESULT=%DOWNLOAD_RESULT% >> properties.properties
echo %PARAM%_TEST_RESULT=%TEST_RESULT% >> properties.properties
echo %PARAM%_STEP_RESULT=%STEP_RESULT% >> properties.properties
echo.>> properties.properties

if %STEP_RESULT% neq 0 (endlocal & set "FINAL_RESULT=1")
