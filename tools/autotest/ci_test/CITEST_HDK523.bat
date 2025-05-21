@echo off

call tools\autotest\ci_test\sdk_citest_env_init.bat SIFLI_6A eh-lb523_hcpu 5 0 SF32LB52X_NOR

call tools\autotest\ci_test\sdk_citest_common.bat UART keil example\uart\project\common

call tools\autotest\ci_test\sdk_citest_common.bat HAL keil example\hal_example\project\common

call tools\autotest\ci_test\sdk_citest_common.bat BLINK_RTT keil example\get-started\blink\rtt\project

call tools\autotest\ci_test\sdk_citest_common.bat BLINK_NO_OS keil example\get-started\blink\no-os\project

call tools\autotest\ci_test\sdk_citest_common.bat HELLO_WORLD_RTT keil example\get-started\hello_world\rtt\project

call tools\autotest\ci_test\sdk_citest_common.bat HELLO_WORLD_NO_OS keil example\get-started\hello_world\no-os\project

call tools\autotest\ci_test\sdk_citest_common.bat BSP keil test\drivers\project\52x_hdk\hcpu_523

python tools\autotest\ci_test\sdk_citest_analysis.py Z:\CITEST_SDK\%JOB_BASE_NAME%\%BUILD_ID%_%W_BRANCH_NAME:~7,50%_%GIT_COMMIT:~0,8%\
