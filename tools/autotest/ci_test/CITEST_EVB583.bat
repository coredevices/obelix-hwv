@echo off

call tools\autotest\ci_test\sdk_citest_env_init.bat SIFLI_1A ec-lb583_hcpu 16 15 SF32LB58X

call tools\autotest\ci_test\sdk_citest_common.bat UART keil example\uart\project\common

::call tools\autotest\ci_test\sdk_citest_common.bat FS keil example\storage\file_system\project

call tools\autotest\ci_test\sdk_citest_common.bat HAL keil example\hal_example\project\common

call tools\autotest\ci_test\sdk_citest_common.bat IPC keil example\multicore\ipc_queue\common\hcpu

call tools\autotest\ci_test\sdk_citest_common.bat DS keil example\multicore\data_service\common\hcpu

call tools\autotest\ci_test\sdk_citest_common.bat RPMSG keil example\multicore\rpmsg-lite\project\hcpu

call tools\autotest\ci_test\sdk_citest_common.bat BLINK_RTT keil example\get-started\blink\rtt\project

call tools\autotest\ci_test\sdk_citest_common.bat BLINK_NO_OS keil example\get-started\blink\no-os\project

call tools\autotest\ci_test\sdk_citest_common.bat HELLO_WORLD_RTT keil example\get-started\hello_world\rtt\project

call tools\autotest\ci_test\sdk_citest_common.bat HELLO_WORLD_NO_OS keil example\get-started\hello_world\no-os\project

call tools\autotest\ci_test\sdk_citest_common.bat BSP keil test\drivers\project\ec-lb58xxxxxx001\v11_583

python tools\autotest\ci_test\sdk_citest_analysis.py Z:\CITEST_SDK\%JOB_BASE_NAME%\%BUILD_ID%_%W_BRANCH_NAME:~7,50%_%GIT_COMMIT:~0,8%\
