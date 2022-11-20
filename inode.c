/*
 * no copyright because I am lazy
 *
 */

#include <linux/module.h>
#include <linux/fs.h>
#include "wangdehuifs.h"

extern struct inode *wangdehuifs_get_inode(struct super_block *sb,
										int mode, dev_t dev);

static int wdh_mknod(struct inode *dir, struct dentry *dentry,
					int mode, dev_t dev)
{
	struct inode *inode = wangdehuifs_get_inode(dir->i_sb, mode, dev);
	int error = -ENOSPC;
	
	printk(KERN_INFO "wangdehuifs: mknod\n");
	if (inode){
		if (dir->i_mode & S_ISGID) {
			inode->i_gid = dir->i_gid;
			if (S_ISDIR(mode))
				inode->i_mode |= S_ISGID;
		}
		d_instantiate(dentry, inode);
		dget(dentry); /* extra count - pin the dentry in core*/
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;

		/* real filesystem would normally use i_size write function */
		dir->i_size += 0x20; /* bogus small size for each dir entry */
	}
	return error;
}

static int wdh_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	int retval = 0;
	retval = wdh_mknod(dir, dentry, mode | S_IFDIR, 0);

	/* link count is two for dir, for do not */
	//if (!retval)
		// dir->i_nlink++;
	return retval;

}

static int wdh_create(struct inode *dir, struct dentry *dentry, int mode, struct nameidata * nd)
{
	return wdh_mknod(dir, dentry, mode | S_IFREG, 0);
}

static int wdh_symlink(struct inode *dir, struct dentry *dentry, const char *symname)
{	
	struct inode *inode;
	int error = -ENOSPC;
	inode = wangdehuifs_get_inode(dir->i_sb, S_IFLNK | S_IRWXUGO, 0);
	if(inode) {
		int l = strlen(symname) + 1;
		error = page_symlink(inode, symname, l);
		if (!error) {
			if (dir->i_mode & S_ISGID)
				inode->i_gid = dir->i_gid;
			d_instantiate(dentry, inode);
			dget(dentry);
			dir->i_mtime = dir->i_ctime = CURRENT_TIME;
		}else
			iput(inode);
	}
	return error;
}

struct inode_operations wdh_file_inode_ops = {
	.getattr              = simple_getattr,
};

struct inode_operations wdh_dir_inode_ops = {
	.create        = wdh_create,
	.lookup           = simple_lookup,
	.link             = simple_link,
	.unlink           = simple_unlink,
	.symlink          = wdh_symlink,
	.mkdir            = wdh_mkdir,
	.mknod            = wdh_mknod,
	.rmdir            = simple_rmdir,
	.rename           = simple_rename,
};






