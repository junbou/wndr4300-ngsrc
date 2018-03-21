/*
 * DNI minidlna config.h
 */
#ifndef _DNI_MINIDLNA_CONFIG_H
#define _DNI_MINIDLNA_CONFIG_H

#define OS_NAME		"Linux"
#define OS_VERSION	"Linux/2.6"
#define OS_URL		"http://www.netgear.com/"

/* full path of the file database */
#define DEFAULT_DB_PATH	"/tmp/minidlna"

/* full path of the log directory */
#define DEFAULT_LOG_PATH	"/tmp/minidlna"

/* Comment the following line to use home made daemonize() func instead
 * of BSD daemon() */
#define USE_DAEMON

/* Enable if the system inotify.h exists.  Otherwise our own inotify.h will be used. */
#define HAVE_INOTIFY_H

/* Enable if the system iconv.h exists.  ID3 tag reading in various character sets will not work properly otherwise. */
#define HAVE_ICONV_H

/* Enable if the system libintl.h exists for NLS support. */
/*#define ENABLE_NLS*/

/* Enable NETGEAR-specific tweaks. */
#define NETGEAR

/* Enable ReadyNAS-specific tweaks. */
/*#define READYNAS*/

/* Compile in TiVo support. */
#define TIVO_SUPPORT

@TOP@
@BOTTOM@
#endif
