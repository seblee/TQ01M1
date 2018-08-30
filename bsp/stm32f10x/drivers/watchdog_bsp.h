#ifndef _WATCHDOG_
#define _WATCHDOG_

#ifdef _WATCHDOG_H
#define _WATCHDOG_DEF_
#else
#define _WATCHDOG_DEF_ extern
#endif

void watchdog_init(void);

void dog(void);


#endif

