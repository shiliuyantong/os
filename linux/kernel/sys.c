/*
 *  linux/kernel/sys.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <errno.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <signal.h>
// TODO
#define BUF_MAX 256
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
// #include <dirent.h>

int sys_ftime()
{
    return -ENOSYS;
}

int sys_break()
{
    return -ENOSYS;
}

int sys_ptrace()
{
    return -ENOSYS;
}

int sys_stty()
{
    return -ENOSYS;
}

int sys_gtty()
{
    return -ENOSYS;
}

int sys_rename()
{
    return -ENOSYS;
}

int sys_prof()
{
    return -ENOSYS;
}

int sys_setregid(int rgid, int egid)
{
    if (rgid > 0)
    {
        if ((current->gid == rgid) ||
            suser())
            current->gid = rgid;
        else
            return (-EPERM);
    }
    if (egid > 0)
    {
        if ((current->gid == egid) ||
            (current->egid == egid) ||
            suser())
        {
            current->egid = egid;
            current->sgid = egid;
        }
        else
            return (-EPERM);
    }
    return 0;
}

int sys_setgid(int gid)
{
    /*	return(sys_setregid(gid, gid)); */
    if (suser())
        current->gid = current->egid = current->sgid = gid;
    else if ((gid == current->gid) || (gid == current->sgid))
        current->egid = gid;
    else
        return -EPERM;
    return 0;
}

int sys_acct()
{
    return -ENOSYS;
}

int sys_phys()
{
    return -ENOSYS;
}

int sys_lock()
{
    return -ENOSYS;
}

int sys_mpx()
{
    return -ENOSYS;
}

int sys_ulimit()
{
    return -ENOSYS;
}

int sys_time(long *tloc)
{
    int i;

    i = CURRENT_TIME;
    if (tloc)
    {
        verify_area(tloc, 4);
        put_fs_long(i, (unsigned long *)tloc);
    }
    return i;
}

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
int sys_setreuid(int ruid, int euid)
{
    int old_ruid = current->uid;

    if (ruid > 0)
    {
        if ((current->euid == ruid) ||
            (old_ruid == ruid) ||
            suser())
            current->uid = ruid;
        else
            return (-EPERM);
    }
    if (euid > 0)
    {
        if ((old_ruid == euid) ||
            (current->euid == euid) ||
            suser())
        {
            current->euid = euid;
            current->suid = euid;
        }
        else
        {
            current->uid = old_ruid;
            return (-EPERM);
        }
    }
    return 0;
}

int sys_setuid(int uid)
{
    /*	return(sys_setreuid(uid, uid)); */
    if (suser())
        current->uid = current->euid = current->suid = uid;
    else if ((uid == current->uid) || (uid == current->suid))
        current->euid = uid;
    else
        return -EPERM;
    return (0);
}

int sys_stime(long *tptr)
{
    if (!suser())
        return -EPERM;
    startup_time = get_fs_long((unsigned long *)tptr) - jiffies / HZ;
    return 0;
}

int sys_times(struct tms *tbuf)
{
    if (tbuf)
    {
        verify_area(tbuf, sizeof *tbuf);
        put_fs_long(current->utime, (unsigned long *)&tbuf->tms_utime);
        put_fs_long(current->stime, (unsigned long *)&tbuf->tms_stime);
        put_fs_long(current->cutime, (unsigned long *)&tbuf->tms_cutime);
        put_fs_long(current->cstime, (unsigned long *)&tbuf->tms_cstime);
    }
    return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
    if (end_data_seg >= current->end_code &&
        end_data_seg < current->start_stack - 16384)
        current->brk = end_data_seg;
    return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
    int i;

    if (!pid)
        pid = current->pid;
    if (!pgid)
        pgid = current->pid;
    for (i = 0; i < NR_TASKS; i++)
        if (task[i] && task[i]->pid == pid)
        {
            if (task[i]->leader)
                return -EPERM;
            if (task[i]->session != current->session)
                return -EPERM;
            task[i]->pgrp = pgid;
            return 0;
        }
    return -ESRCH;
}

int sys_getpgrp(void)
{
    return current->pgrp;
}

int sys_setsid(void)
{
    if (current->leader && !suser())
        return -EPERM;
    current->leader = 1;
    current->session = current->pgrp = current->pid;
    current->tty = -1;
    return current->pgrp;
}

int sys_getgroups()
{
    return -ENOSYS;
}

int sys_setgroups()
{
    return -ENOSYS;
}

int sys_uname(struct utsname *name)
{
    static struct utsname thisname = {
        "linux .0", "nodename", "release ", "version ", "machine "};
    int i;

    if (!name)
        return -ERROR;
    verify_area(name, sizeof *name);
    for (i = 0; i < sizeof *name; i++)
        put_fs_byte(((char *)&thisname)[i], i + (char *)name);
    return 0;
}

int sys_sethostname()
{
    return -ENOSYS;
}

int sys_getrlimit()
{
    return -ENOSYS;
}

int sys_setrlimit()
{
    return -ENOSYS;
}

int sys_getrusage()
{
    return -ENOSYS;
}

int sys_gettimeofday()
{
    return -ENOSYS;
}

int sys_settimeofday()
{
    return -ENOSYS;
}

int sys_umask(int mask)
{
    int old = current->umask;

    current->umask = mask & 0777;
    return (old);
}

int sys_getcwd(char *buf, size_t size)
{
    if (buf == NULL)
        buf = (char *)malloc(size);

    char path[BUF_MAX], cwd[BUF_MAX];
    struct buffer_head *bh;
    struct m_inode *inode_cur, *inode_root;
    struct dir_entry *dir_cur, *dir_temp;
    unsigned short num_inode_cur, num_inode_par;
    int block_cur, idev_cur, entries, i;
    memset(path, 0, sizeof(path));

    inode_cur = current->pwd;
    inode_root = current->root;
    block_cur = inode_cur->i_zone[0];
    idev_cur = inode_cur->i_dev;

    bh = bread(idev_cur, block_cur);
    dir_cur = (struct dir_entry *)(bh->b_data);
    num_inode_cur = dir_cur->inode;
    num_inode_par = (++dir_cur)->inode;

    while (inode_cur != inode_root)
    {
        inode_cur = iget(idev_cur, num_inode_par);
        block_cur = inode_cur->i_zone[0];
        idev_cur = inode_cur->i_dev;
        bh = bread(idev_cur, block_cur);
        dir_cur = (struct dir_entry *)(bh->b_data);
        dir_temp = dir_cur;

        entries = inode_cur->i_size / sizeof(struct dir_entry);
        for (i = 0; i < entries; i++, dir_temp++)
        {
            if (dir_temp->inode == num_inode_cur)
            {
                memset(cwd, 0, sizeof(cwd));

                strcat(cwd, "/");
                strcat(cwd, dir_temp->name);
                strcat(cwd, path);
                strcpy(path, cwd);

                break;
            }
        }

        num_inode_cur = num_inode_par;
        num_inode_par = (++dir_cur)->inode;
    }

    size_t len = strlen(path);
    char *p = buf;
    for (i = 0; i < len; ++i)
        put_fs_byte(path[i], p++);

    return buf;
}

// long sys_getcwd(char *buf, size_t size)
// {
//     if (buf == NULL)
//     {
//         buf = (char *)malloc(size);
//     }

//     char path[BUF_MAX], cwd[BUF_MAX];
//     DIR *dirp;                     // dir_info
//     struct dirent *dp;       // dir_file_info
//     struct stat *sb, *sb_p, *sb_c; // stat buff(detailed_file_info)
//     dev_t dev;
//     ino_t ino;

//     while (1)
//     {
//         // Current dir_info
//         stat(".", sb);
//         dev = sb->st_dev;
//         ino = sb->st_ino;

//         // Parent
//         dirp = opendir("..");
//         stat("..", sb_p);

//         // Whether reach
//         if (sb_p->st_dev == dev && sb_p->st_ino == ino)
//             break;

//         // Search in dir_parent
//         while ((dp->name = readdir(dirp)) != NULL)
//         {
//             snprintf(path, BUF_MAX, "../%s", dp->d_name);
//             stat(path, sb_c);

//             // Find!
//             if (sb_c->st_dev == dev && sb_c->st_ino == ino)
//             {
//                 memset(cwd, 0, sizeof(cwd));

//                 stract(cwd, "/");
//                 strcat(cwd, dp->d_name);
//                 strcat(cwd, buf);
//                 strncpy(buf, cwd, BUF_MAX);

//                 break;
//             }
//         }

//         // Change dir_grandparent
//         closedir(dirp);
//         chdir("..");
//     }

//     return buf;
// }