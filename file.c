/* 
 * no copyright because I am lazy!
 *
 */

#include <linux/module.h>
#include <linux/fs.h>

struct address_space_operations wdh_aops = {
	.readpage        = simple_readpage,
	.write_begin     = simple_write_begin,
	.write_end       = simple_write_end,
};

struct file_operations wdh_file_operations = {
	.read            = do_sync_read,
	.aio_read        = generic_file_aio_read,
	.write           = do_sync_write,
	.aio_write       = generic_file_aio_write,
	.mmap            = generic_file_mmap,
	.fsync           = noop_fsync,
	.llseek          = generic_file_llseek,
};
