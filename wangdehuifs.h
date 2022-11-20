/*
 * fs/wangdehuifs/wangdehuifs.h
 *
 * no copyright because I am lazy
 *
 * write date: 2022/11/20 15:00:00
 */

#define WANGDEHUIFS_ROOT_I 2

/* This os a example of fs specific mount data that a file system might want to store.
 *
 * fs per-superblock data varies widely and some fs do not require any information beyond the generic info which is 
 * already in struct super_block *
 */

 struct wangdehuifs_sb_info {
   int wangdehuifs_var1;
   int wangdehuifs_var2;
 };

 static inline struct wangdehuifs_sb_info *
 WDH_SB(struct super_block *sb)
 {
	return sb->s_fs_info;
 }

