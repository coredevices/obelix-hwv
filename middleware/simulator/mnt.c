/*
 * File      : mnt.c
 * COPYRIGHT (C) 2008 - 2016, RT-Thread Development Team
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-4-3       Urey         the first version
 */

#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>

int mnt_init(void)
{
    dfs_init();

    extern int dfs_win32_init(void);
    extern rt_err_t rt_win_sharedir_init(const char *name);

    dfs_win32_init();
    rt_win_sharedir_init("wshare");

    if (dfs_mount("wshare", "/", "wdir", 0, 0) == 0)
    {
        rt_kprintf("File System on root initialized!\n");
    }
    else
    {
        rt_kprintf("File System on root initialization failed!\n");
    }

    return 0;
}

#endif
