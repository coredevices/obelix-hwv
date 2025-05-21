@echo off
echo.
echo SDK自动化测试任务启动，输入参数如下：--------------------------------------
echo.
echo ----------------------- info ---------------------------------
echo WORKSPACE: %WORKSPACE%
echo JOB_BASE_NAME: %JOB_BASE_NAME%
echo GIT_COMMIT: %GIT_COMMIT:~0,8%
echo GIT_COMMITTER_NAME: %GIT_COMMITTER_NAME%
echo GIT_AUTHOR_NAME: %GIT_AUTHOR_NAME%
echo BUILD_USER_ID: %BUILD_USER_ID%
echo BUILD_USER_EMAIL: %BUILD_USER_EMAIL%
echo BRANCH_NAME: %BRANCH_NAME%
echo ----------------------- info ---------------------------------

echo.
echo ----------------------- Get uart info ---------------------------------
echo %Get_port%
%Get_port% %1
set UART=com%ERRORLEVEL%
ECHO %UART%
echo.
echo ----------------------- Set Test info ---------------------------------
set TEST_DIR=%WORKSPACE%_Test
set DownloadTool=%WORKSPACE%\tools\uart_download\
set PRJ_NAME=%2
set RELAY_PORT=com3
set UART_PORT=%UART%
set RTYPE=0
set RCHAN=%3
set BCHAN=%4
set DEVICE_TYPE=%5
set W_BRANCH_NAME=%BRANCH_NAME:/=_%

echo.
echo  -------------- Print Test info --------------
echo TEST_DIR -- %TEST_DIR%
echo DownloadTool -- %DownloadTool%
echo PRJ_NAME -- %PRJ_NAME%
echo UART_PORT -- %UART_PORT%
echo RELAY_PORT -- %RELAY_PORT%
echo RTYPE -- %RTYPE%
echo RCHAN-- %RCHAN%
echo BCHAN -- %BCHAN%
echo DEVICE_TYPE-- %DEVICE_TYPE%
echo W_BRANCH_NAME=%W_BRANCH_NAME%

echo.
echo  -------------- Inject Test info --------------
echo TEST_DIR=%TEST_DIR% > properties.properties
echo DownloadTool=%DownloadTool% >> properties.properties
echo PRJ_NAME=%PRJ_NAME% >> properties.properties
echo UART_PORT=%UART_PORT% >> properties.properties
echo RELAY_PORT=%RELAY_PORT% >> properties.properties
echo RTYPE=%RTYPE% >> properties.properties
echo RCHAN=%RCHAN% >> properties.properties
echo BCHAN=%BCHAN% >> properties.properties
echo DEVICE_TYPE=%DEVICE_TYPE% >> properties.properties
echo W_BRANCH_NAME=%W_BRANCH_NAME% >> properties.properties
mkdir "%WORKSPACE%_Test\Result\%BUILD_ID%_%BUILD_USER_ID%_%W_BRANCH_NAME:~7,50%_%GIT_COMMIT:~0,8%"


