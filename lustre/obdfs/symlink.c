/*
 *  linux/fs/ext2/symlink.c
 *
 * This code is issued under the GNU General Public License.
 * See the file COPYING in this distribution
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/symlink.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  ext2 symlink handling code
 *
 * Modified for OBDFS: 
 *  Copyright (C) 1999 Seagate Technology Inc. (author: braam@stelias.com)
 */

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/stat.h>
#include <linux/locks.h>
#include <linux/obd_support.h> /* for ENTRY and EXIT only */
#include <linux/obdfs.h>

static int obdfs_fast_readlink(struct dentry *dentry, char *buffer, int buflen)
{
        char *s = obdfs_i2info(dentry->d_inode)->oi_inline;
        return vfs_readlink(dentry, buffer, buflen, s);
}

static int obdfs_fast_follow_link(struct dentry *dentry, struct nameidata *nd)
{
        char *s = obdfs_i2info(dentry->d_inode)->oi_inline;
        return vfs_follow_link(nd, s); 
}

struct inode_operations obdfs_fast_symlink_inode_operations = {
        readlink:       obdfs_fast_readlink,
        follow_link:    obdfs_fast_follow_link,
};

static int obdfs_readlink(struct dentry *dentry, char *buffer, int buflen)
{
        struct page *page = NULL;
        int res;

        ENTRY;
        OIDEBUG(dentry->d_inode);
        page = obdfs_getpage(dentry->d_inode, 0, 0, 0);
        /* PDEBUG(page, "readlink"); */
        if (!page) {
                EXIT;
                return 0;
        }
        res = vfs_readlink(dentry, buffer, buflen, (char *)page_address(page));
        page_cache_release(page);
        EXIT;
        return res;
} /* obdfs_readlink */

static int obdfs_follow_link(struct dentry * dentry,
                             struct nameidata *nd)
{
        struct page *page = NULL;
        int res;

        ENTRY;
        OIDEBUG(dentry->d_inode);
        page = obdfs_getpage(dentry->d_inode, 0, 0, 0);
        /* PDEBUG(page, "follow_link"); */
        if (!page) {
                dput(nd->dentry);
                EXIT;
                return -EIO;
        }
        res = vfs_follow_link(nd, (char *)page_address(page));
        page_cache_release(page);
        EXIT;
        return res;
}

struct inode_operations obdfs_symlink_inode_operations = {
        readlink:       obdfs_readlink,
        follow_link:    obdfs_follow_link,
};
