/* udev-106 hotplug2-0.9
  *
  * add@/block/sdc					DEVPATH=/block/sdc
  * add@/block/sdc/sdc1			DEVPATH=/block/sdc/sdc1
  * add@/block/sdc/sdc2			DEVPATH=/block/sdc/sdc2
  * add@/class/scsi_generic/sg2		DEVPATH=/class/scsi_generic/sg2
  *
  * 7052 ==================== sda1 ====================
  * 7054 ==================== sdb3 ====================
  * 7055 ==================== sdb5 ====================
  * 7056 ==================== sdb6 ====================
  * 7057 ==================== sdb7 ====================
  * 7058 ==================== sdb8 ====================
  * 7060 ==================== sdc1 ====================
  * 7061 ==================== sdc2 ====================
  * 7106 ==================== sg0 ====================
  * 7107 ==================== sg1 ====================
  * 7108 ==================== sg2 ====================
  *
  */

#include "hotplug.h"

struct list_head disks_list = LIST_HEAD_INIT(disks_list);

static void sigchld(int sig)
{
	pid_t pid;
	int status;

	for (;;) {
		pid = waitpid(-1, &status, WNOHANG);
		if (pid <= 0)
			break;
		DEBUGP("NET-DISK: Reaping child %d: status %d\n", pid, status);
	}
}

static int init_netlink_socket(void)
{
	int netlink_socket;
	struct sockaddr_nl snl;
	int buffersize = 2 * 1024 * 1024;

	memset(&snl, 0x00, sizeof(struct sockaddr_nl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;
	netlink_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT); 
	if (netlink_socket == -1) {
		DEBUGP("Failed socket: %s.\n", strerror(errno));
		return -1;
	}

	/* We're trying to override buffer size. If we fail, we attempt to set a big buffer and pray. */
	if (setsockopt(netlink_socket, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize))) {
		DEBUGP("Failed setsockopt SO_RCVBUFFORCE: %s. (non-critical)\n", strerror(errno));

		/* Somewhat safe default. */
		buffersize = 106496;
		setsockopt(netlink_socket, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));
	}

	if (bind(netlink_socket, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl))) {
		DEBUGP("Failed bind: %s.\n", strerror(errno));
		close(netlink_socket);
		return -1;
	}

	return netlink_socket;
}

static void run_action(struct udisk_info *udisk)
{
	char cmd[256];
	struct list_head *pos;
	struct upartition_info *upart;

	system("/sbin/cmddlna stop");
	/* wait some time before starting to mount partitions ... */
	sleep(2);

	printf("\n\n");
	list_for_each(pos, &udisk->node) {
		upart = list_entry(pos, struct upartition_info, list);
		printf("[*** USB Storage ***] Mounting USB Disk Partition %s ...\n", upart->name);

		snprintf(cmd, sizeof(cmd), "/sbin/hotplug2.mount %s", upart->name);
		system(cmd);
		snprintf(cmd, sizeof(cmd), "/usr/sbin/usb_cfg approve %s", upart->name);
		system(cmd);
	}

	if (list_empty(&udisk->node)) { /* No partitions, try to use disk name such as 'sda' to mount ... */
		printf("[*** USB Storage ***] Mounting USB Disk Partition %s ...\n", udisk->disk_name);
	
	snprintf(cmd, sizeof(cmd), "/sbin/hotplug2.mount %s", udisk->disk_name);
	system(cmd);
	snprintf(cmd, sizeof(cmd), "/usr/sbin/usb_cfg approve %s", udisk->disk_name);
	system(cmd);
	}

	printf("[*** USB Storage ***] Mounted the Disk %s <--> %s successfully!!!\n",
				udisk->disk_name, udisk->scsi_name);

	/* Turn usbled on/off */
	system("/usr/bin/killall usbled");
	system("/usr/sbin/usbled > /dev/null");

	/* Update the share information when the full disk is mounted. */
	snprintf(cmd, sizeof(cmd), "/usr/sbin/update_smb %s", udisk->disk_name);
	system(cmd);
	/* Update the AFP share information when the full disk is mounted. */
        snprintf(cmd, sizeof(cmd), "/usr/sbin/update_afp %s", udisk->disk_name);
        system(cmd);
	
	system("/sbin/cmdftp start 2> /dev/null");
	system("/usr/sbin/chkfuppes 2> /dev/null");
	system("/sbin/cmddlna start 2> /dev/null");

	exit(0);
}

static void run_mounting_disk(void)
{
	struct udisk_info *udisk;
	struct upartition_info *upart;
	struct list_head *pos, *n, *iter, *nxt;

	list_for_each_safe(pos, n, &disks_list) {
		udisk = list_entry(pos, struct udisk_info, list);
		if (!udisk->finished)
			continue;

		if (fork() == 0)
			run_action(udisk);
		list_del(pos);
		list_for_each_safe(iter, nxt, &udisk->node) {
			upart = list_entry(iter, struct upartition_info, list);
			list_del(iter);
			free(upart);
		}
		free(udisk);
	}
}

#if MOUNT_TIME_CHECK
void mount_timeover_handle(int sig)
{
#define INIT_SCSI_NAME "WAIT"
	struct list_head *pos;
	struct udisk_info *udisk;

	list_for_each(pos, &disks_list) {
		udisk = list_entry(pos, struct udisk_info, list);
		if (strcmp(udisk->scsi_name, INIT_SCSI_NAME) == 0) {
			snprintf(udisk->scsi_name, sizeof(udisk->scsi_name), "%s", "X-DISK");
			udisk->finished = 1;
		}
	}

	run_mounting_disk();
}
#endif

int main(int argc, char **argv)
{
	int size;
	int netlink_socket;
	static char buffer[UEVENT_BUFFER_SIZE + 512];

	netlink_socket = init_netlink_socket();
	if (netlink_socket == -1) {
		printf("Failed to open NETLINK_KOBJECT_UEVENT netlink socket!\n");
		return -1;
	}

	printf("USB Storage daemon is Running ... \n");
	daemon(1, 1);
	signal(SIGCHLD, sigchld);

	while (1) {
		size = recv(netlink_socket, buffer, sizeof(buffer) - 1, 0);
		if (size <  0) {
			if (errno != EINTR)
				printf("Unable to receive kernel netlink message: %s\n", strerror(errno));
			continue;
		}

		buffer[size] = '\0';
		DEBUGC(buffer, size);
		
		parse_uevent_msg(buffer, size);
		run_mounting_disk();
	}

	return 0;
}

