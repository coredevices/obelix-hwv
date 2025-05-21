@echo off

echo.
echo SDK自动化测试任务启动，输入参数如下：--------------------------------------

::echo ----------------------- user configure info ---------------------------------
set RELAY_PORT=COM3
set UART_BURN_DES=SIFLI_5A
set UART_H_DES=SIFLI_5B
set UART_L_DES=SIFLI_5A
set RTYPE=0
set RCHAN=12
set BCHAN=11
set DEVICE_TYPE=SF32LB56X_NOR
set PRJ_NAME=em-lb566
::set BSP_PRJ_H
::set BSP_PRJ_L

set FINAL_RESULT=0
set TEST_DIR=%WORKSPACE%_Test
set DownloadTool=%WORKSPACE%\tools\uart_download\
set PlanExpTool=%WORKSPACE%\tools\autotest\ci_test2\SdkCiTask.exe
set W_BRANCH_NAME=%BRANCH_NAME:/=_%
%Get_port% %UART_H_DES%
set UART_H_PORT=COM%ERRORLEVEL%
%Get_port% %UART_L_DES%
set UART_L_PORT=COM%ERRORLEVEL%
%Get_port% %UART_BURN_DES%
set UART_BURN_PORT=COM%ERRORLEVEL%


echo.
echo ----------------------- build info ---------------------------------
echo WORKSPACE: %WORKSPACE%
echo JOB_BASE_NAME: %JOB_BASE_NAME%
echo GIT_COMMIT: %GIT_COMMIT:~0,8%
echo GIT_COMMITTER_NAME: %GIT_COMMITTER_NAME%
echo GIT_AUTHOR_NAME: %GIT_AUTHOR_NAME%
echo BUILD_USER_ID: %BUILD_USER_ID%
echo BUILD_USER_EMAIL: %BUILD_USER_EMAIL%
echo BRANCH_NAME: %BRANCH_NAME%

echo.
echo  -------------- Print Test info --------------
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
echo  -------------- Inject Test info --------------
echo TEST_DIR=%TEST_DIR% > properties.properties
echo DownloadTool=%DownloadTool% >> properties.properties
echo PRJ_NAME=%PRJ_NAME% >> properties.properties
echo UART_H_PORT=%UART_H_PORT% >> properties.properties
echo UART_L_PORT=%UART_L_PORT% >> properties.properties
echo UART_BURN_PORT=%UART_BURN_PORT% >> properties.properties
echo RELAY_PORT=%RELAY_PORT% >> properties.properties
echo RTYPE=%RTYPE% >> properties.properties
echo RCHAN=%RCHAN% >> properties.properties
echo BCHAN=%BCHAN% >> properties.properties
echo DEVICE_TYPE=%DEVICE_TYPE% >> properties.properties
echo W_BRANCH_NAME=%W_BRANCH_NAME% >> properties.properties
mkdir "%WORKSPACE%_Test\Result\%BUILD_ID%_%BUILD_USER_ID%_%W_BRANCH_NAME:~7,50%_%GIT_COMMIT:~0,8%"

echo ******************************
%PlanExpTool% --src %TEST_RESULT_EXCEL% --board %PRJ_NAME% --dst %WORKSPACE%\tools\autotest\ci_test2
echo TEST_RESULT_EXCEL -- %TEST_RESULT_EXCEL%
::copy TEST_CFG %WORKSPACE%\%TEST_CFG%
call tools\autotest\ci_test2\%PRJ_NAME%.bat
exit /b %FINAL_RESULT%


::call tools\autotest\ci_test\sdk_citest_env_init.bat SIFLI_5A em-lb566_hcpu 12 11 SF32LB56X_NOR

::call tools\autotest\ci_test\sdk_citest_common.bat UART keil example\uart\project\common

::call tools\autotest\ci_test\sdk_citest_common.bat FS keil example\storage\file_system\project

::call tools\autotest\ci_test\sdk_citest_common.bat HAL keil example\hal_example\project\common

::call tools\autotest\ci_test\sdk_citest_common.bat IPC keil example\multicore\ipc_queue\common\hcpu

::call tools\autotest\ci_test\sdk_citest_common.bat DS keil example\multicore\data_service\common\hcpu

::call tools\autotest\ci_test\sdk_citest_common.bat RPMSG keil example\multicore\rpmsg-lite\project\hcpu

::call tools\autotest\ci_test\sdk_citest_common.bat BLINK_RTT keil example\get-started\blink\rtt\project

::call tools\autotest\ci_test\sdk_citest_common.bat BLINK_NO_OS keil example\get-started\blink\no-os\project

::call tools\autotest\ci_test\sdk_citest_common.bat HELLO_WORLD_RTT keil example\get-started\hello_world\rtt\project

::call tools\autotest\ci_test\sdk_citest_common.bat HELLO_WORLD_NO_OS  example\get-started\hello_world\no-os\project

::python tools\autotest\ci_test\sdk_citest_analysis.py Z:\CITEST_SDK\%JOB_BASE_NAME%\%BUILD_ID%_%W_BRANCH_NAME:~7,50%_%GIT_COMMIT:~0,8%\
