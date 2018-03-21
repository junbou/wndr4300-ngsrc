#ifndef __WNDR4300_USBLED_H
#define __WNDR4300_USBLED_H

#include <linux/version.h>
#include <linux/list.h>
#define SWITCH_MAX_BUFSZ	4096
#define USBLED_MAX_BUFSZ	4096

#define SWITCH_MEDIA_AUTO	1
#define SWITCH_MEDIA_100	2
#define SWITCH_MEDIA_FD		4

#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#define LINUX_2_4
#endif

typedef int (*usbled_handler)(void *driver, char *buf, int nr);

typedef struct {
	const char *name;
	usbled_handler read, write;
} usbled_config;

#endif
