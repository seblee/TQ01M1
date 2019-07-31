#include <rtthread.h>
#include "sys_conf.h"
#include "team.h"

// void team_thread_entry(void *parameter)
// {
//     rt_thread_delay(TEAM_THREAD_DELAY);
//     // rt_kprintf("team_thread_entry successfully.\n");
//     while (1)
//     {
//         team_fsm_trans();
//         rt_thread_delay(2000);
//     }
// }
