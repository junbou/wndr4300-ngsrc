/*
 * Copyright (c) 1997-2003 Erez Zadok
 * Copyright (c) 2001-2003 Stony Brook University
 *
 * For specific licensing information, see the COPYING file distributed with
 * this package, or get one from ftp://ftp.filesystems.org/pub/fist/COPYING.
 *
 * This Copyright notice must be kept intact and distributed with all
 * fistgen sources INCLUDING sources generated by fistgen.
 */
/*
 * Copyright (C) 2004, 2005 Markus Klotzbuecher <mk@creamnet.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

/*
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "fist.h"
#include "mini_fo.h"


STATIC void
mini_fo_read_inode(inode_t *inode)
{
	static struct address_space_operations mini_fo_empty_aops;

	__itopd(inode) = kmalloc(sizeof(struct mini_fo_inode_info), GFP_KERNEL);
	if (!itopd(inode)) {
		printk("<0>%s:%s:%d: No kernel memory!\n", __FILE__, __FUNCTION__, __LINE__);
		ASSERT(NULL);
	}
	itohi(inode) = NULL;
	itohi2(inode) = NULL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	inode->i_version++;
#else
	inode->i_version = ++event;	/* increment inode version */
#endif
	inode->i_op = &mini_fo_main_iops;
	inode->i_fop = &mini_fo_main_fops;
#if 0
	/*
	 * XXX: To export a file system via NFS, it has to have the
	 * FS_REQUIRES_DEV flag, so turn it on.  But should we inherit it from
	 * the lower file system, or can we allow our file system to be exported
	 * even if the lower one cannot be natively exported.
	 */
	inode->i_sb->s_type->fs_flags |= FS_REQUIRES_DEV;
	/*
	 * OK, the above was a hack, which is now turned off because it may
	 * cause a panic/oops on some systems.  The correct way to export a
	 * "nodev" filesystem is via using nfs-utils > 1.0 and the "fsid=" export
	 * parameter, which requires 2.4.20 or later.
	 */
#endif
	/* I don't think ->a_ops is ever allowed to be NULL */
	inode->i_mapping->a_ops = &mini_fo_empty_aops;
}


#if defined(FIST_DEBUG) || defined(FIST_FILTER_SCA)
/*
 * No need to call write_inode() on the lower inode, as it
 * will have been marked 'dirty' anyway. But we might need
 * to write some of our own stuff to disk.
 */
STATIC void
mini_fo_write_inode(inode_t *inode, int sync)
{
	print_entry_location();
	print_exit_location();
}
#endif /* defined(FIST_DEBUG) || defined(FIST_FILTER_SCA) */


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
STATIC void
mini_fo_put_inode(inode_t *inode)
{
	/*
	 * This is really funky stuff:
	 * Basically, if i_count == 1, iput will then decrement it and this inode will be destroyed.
	 * It is currently holding a reference to the hidden inode.
	 * Therefore, it needs to release that reference by calling iput on the hidden inode.
	 * iput() _will_ do it for us (by calling our clear_inode), but _only_ if i_nlink == 0.
	 * The problem is, NFS keeps i_nlink == 1 for silly_rename'd files.
	 * So we must for our i_nlink to 0 here to trick iput() into calling our clear_inode.
	 */
	if (atomic_read(&inode->i_count) == 1)
		inode->i_nlink = 0;
}
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */


#if defined(FIST_DEBUG) || defined(FIST_FILTER_SCA)
/*
 * we now define delete_inode, because there are two VFS paths that may
 * destroy an inode: one of them calls clear inode before doing everything
 * else that's needed, and the other is fine.  This way we truncate the inode
 * size (and its pages) and then clear our own inode, which will do an iput
 * on our and the lower inode.
 */
STATIC void
mini_fo_delete_inode(inode_t *inode)
{
	print_entry_location();

	fist_checkinode(inode, "mini_fo_delete_inode IN");
	inode->i_size = 0;		/* every f/s seems to do that */
	clear_inode(inode);

	print_exit_location();
}
#endif /* defined(FIST_DEBUG) || defined(FIST_FILTER_SCA) */


/* final actions when unmounting a file system */
STATIC void
mini_fo_put_super(super_block_t *sb)
{
	if (stopd(sb)) {
		mntput(stopd(sb)->hidden_mnt);
		mntput(stopd(sb)->hidden_mnt2);

		/* mk: no! dput(stopd(sb)->base_dir_dentry);
		   dput(stopd(sb)->storage_dir_dentry); */

		kfree(stopd(sb));
		__stopd(sb) = NULL;
	}
}


#ifdef NOT_NEEDED
/*
 * This is called in do_umount before put_super.
 * The superblock lock is not held yet.
 * We probably do not need to define this or call write_super
 * on the hidden_sb, because sync_supers() will get to hidden_sb
 * sooner or later.  But it is also called from file_fsync()...
 */
STATIC void
mini_fo_write_super(super_block_t *sb)
{
	return;
}
#endif /* NOT_NEEDED */


STATIC int
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
mini_fo_statfs(struct dentry *d, struct kstatfs *buf)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
mini_fo_statfs(super_block_t *sb, struct kstatfs *buf)
#else
mini_fo_statfs(super_block_t *sb, struct statfs *buf)
#endif
{
	int err = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
	struct dentry *hidden_d;

	hidden_d = dtohd(d);
	err = vfs_statfs(hidden_d, buf);
#else
	super_block_t *hidden_sb;

	hidden_sb = stohs(sb);
	err = vfs_statfs(hidden_sb, buf);
#endif

	return err;
}


/*
 * XXX: not implemented.  This is not allowed yet.
 * Should we call this on the hidden_sb?  Probably not.
 */
STATIC int
mini_fo_remount_fs(super_block_t *sb, int *flags, char *data)
{
	//printk(KERN_CRIT "mini_fo_remount_fs: WARNING, this function is umimplemented.\n");
	return -ENOSYS;
}


/*
 * Called by iput() when the inode reference count reached zero
 * and the inode is not hashed anywhere.  Used to clear anything
 * that needs to be, before the inode is completely destroyed and put
 * on the inode free list.
 */
STATIC void
mini_fo_clear_inode(inode_t *inode)
{
	/*
	 * Decrement a reference to a hidden_inode, which was incremented
	 * by our read_inode when it was created initially.
	 */

	/* release the wol_list */
	if(S_ISDIR(inode->i_mode)) {
		__meta_put_lists(inode);
	}

	/* mk: fan out fun */
	if(itohi(inode))
		iput(itohi(inode));
	if(itohi2(inode))
		iput(itohi2(inode));

	// XXX: why this assertion fails?
	// because it doesn't like us
	// ASSERT((inode->i_state & I_DIRTY) == 0);
	kfree(itopd(inode));
	__itopd(inode) = NULL;
}


/*
 * Called in do_umount() if the MNT_FORCE flag was used and this
 * function is defined.  See comment in linux/fs/super.c:do_umount().
 * Used only in nfs, to kill any pending RPC tasks, so that subsequent
 * code can actually succeed and won't leave tasks that need handling.
 *
 * PS. I wonder if this is somehow useful to undo damage that was
 * left in the kernel after a user level file server (such as amd)
 * dies.
 */
STATIC void
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)) && (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
mini_fo_umount_begin(struct vfsmount *mnt, int flags)
{
	struct vfsmount *hidden_mnt;

	hidden_mnt = stopd(mnt->mnt_sb)->hidden_mnt;

	if (hidden_mnt->mnt_sb->s_op->umount_begin)
		hidden_mnt->mnt_sb->s_op->umount_begin(hidden_mnt, flags);

}
#else
mini_fo_umount_begin(super_block_t *sb)
{
	super_block_t *hidden_sb;

	hidden_sb = stohs(sb);

	if (hidden_sb->s_op->umount_begin)
		hidden_sb->s_op->umount_begin(hidden_sb);

}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
struct inode *
mini_fo_iget(struct super_block *sb, unsigned long ino)
{
	struct inode *inode;

	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);

	if (!(inode->i_state & I_NEW))
		return inode;

	mini_fo_read_inode(inode);

	unlock_new_inode(inode);
	return inode;
}
#endif /* if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25) */

struct super_operations mini_fo_sops =
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	read_inode:		mini_fo_read_inode,
#endif
#if defined(FIST_DEBUG) || defined(FIST_FILTER_SCA)
	write_inode:	mini_fo_write_inode,
#endif /* defined(FIST_DEBUG) || defined(FIST_FILTER_SCA) */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	put_inode:		mini_fo_put_inode,
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */
#if defined(FIST_DEBUG) || defined(FIST_FILTER_SCA)
	delete_inode:	mini_fo_delete_inode,
#endif /* defined(FIST_DEBUG) || defined(FIST_FILTER_SCA) */
	put_super:		mini_fo_put_super,
	statfs:		mini_fo_statfs,
	remount_fs:		mini_fo_remount_fs,
	clear_inode:	mini_fo_clear_inode,
	umount_begin:	mini_fo_umount_begin,
};
