/*
 * fs/wangdehuifs/super.c
 *
 * no copyright because I am lazy!
 *
 * Primitive example to show how to create a Linux fs module 
 *
 * superblock related and misc. functions
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/backing-dev.h>
#include "wangdehuifs.h"

/* helpful if this is diffrent than other fs */
#define WANGDEHUIFS_MAGIC 0x73616d70

extern struct inode_operations wdh_dir_inode_ops;
extern struct inode_operations wdh_file_inode_ops;
extern struct file_operations wdh_file_operations;
extern struct address_space_operations wdh_aops;

static void wangdehuifs_put_super(struct supper_block *sb)
{
	struct wangdehuifs_sb_info *wdh_sb;

	wdh_sb = WDH_SB(sb);
	if (wdh_sb == NULL ) {
		/* Empty superblock info passed to umount */
		return;
	}
	/* FS-FILLIN your fs specific umount logic here */
	kfree(wdh_sb);
	//sb->s_fs_info = NULL;
	return;
}

struct super_operations wangdehuifs_super_ops = {
	.statfs            = simple_statfs,
	.drop_inode        = generic_delete_inode,
	.put_super         = wangdehuifs_put_super,
};


static struct backing_dev_info wdh_backing_dev_info = {
	.ra_pages          = 0,   /* no readahead */
	.capabilities      = BDI_CAP_NO_ACCT_DIRTY | BDI_CAP_NO_WRITEBACK |
		           BDI_CAP_MAP_DIRECT | BDI_CAP_MAP_COPY |
				   BDI_CAP_READ_MAP | BDI_CAP_WRITE_MAP |
				   BDI_CAP_EXEC_MAP,
};

struct inode *wangdehuifs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);

	if (inode) {
		inode->i_mode = mode;
		inode->i_uid  = current_fsuid();
		inode->i_gid  = current_fsgid();
		inode->i_blocks = 0;
		inode->i_atime  = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		inode->i_mapping->a_ops = &wdh_aops;
		inode->i_mapping->backing_dev_info = &wdh_backing_dev_info;
		switch (mode & S_IFMT) {
			default:
				init_special_inode(inode, mode, dev);
				break;
			case S_IFREG:
				printk(KERN_INFO "file inode\n");
				inode->i_op = &wdh_file_inode_ops;
				inode->i_fop = &wdh_file_operations;
				break;
			case S_IFDIR:
				inode->i_op = &wdh_dir_inode_ops;
				inode->i_fop = &simple_dir_operations;

				/* link == 2 (for initial ".." and "." entries */
				//inode->i_nlink++;
				break;
			case S_IFLNK:
				inode->i_op = &page_symlink_inode_operations;
				break;
		}
	}
	return inode;
}

static int wangdehuifs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	struct wangdehuifs_sb_info *wangdehuifs_sb;

	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = WANGDEHUIFS_MAGIC;
	sb->s_op = &wangdehuifs_super_ops;
	sb->s_time_gran = 1; /* 1 nanosecond time granularity */

	printk(KERN_INFO "wangdehuifs fill super");
	root = wangdehuifs_get_inode(sb, S_IFDIR | 0755, 0);
	if (!root)
		return -ENOMEM;

	sb->s_fs_info = kzalloc(sizeof(struct wangdehuifs_sb_info), GFP_KERNEL);
	wangdehuifs_sb = WDH_SB(sb);
	if (!wangdehuifs_sb) {
		iput(root);
		return -ENOMEM;
	}
	printk(KERN_INFO "wangdehuifs: about to alloc dentry root inode\n");
	sb->s_root = d_make_root(root);
	if (!sb->s_root) {
		iput(root);
		kfree(wangdehuifs_sb);
		sb->s_fs_info = NULL;
		return -ENOMEM;
	}
	/* FS-FILLIN your filesystem specific mount logic/checks here */
	return 0;
}


static struct dentry *wangdehuifs_mount(struct file_system_type *fs_type,
					 int flags, const char *dev_name, void *data)
{
	return mount_nodev(fs_type, flags, data, wangdehuifs_fill_super);
}

static void wangdehuifs_kill_sb(struct super_block *sb)
{
	kill_litter_super(sb);
}

static struct file_system_type wangdehuifs_fs_type = {
	.owner = THIS_MODULE,
	.name  = "wangdehuifs",
	.mount = wangdehuifs_mount,
	.kill_sb = wangdehuifs_kill_sb,
	/*  .fs_flags */
};


static int __init init_wangdehuifs_fs(void)
{
	printk(KERN_INFO "init wangdehuifs\n");
	return register_filesystem(&wangdehuifs_fs_type);
}

static void __exit exit_wangdehuifs_fs(void)
{
	printk(KERN_INFO "unloading wangdehuifs\n");
	unregister_filesystem(&wangdehuifs_fs_type);
}

module_init(init_wangdehuifs_fs);
module_exit(exit_wangdehuifs_fs);
MODULE_LICENSE("GPL");


