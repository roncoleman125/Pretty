/* Return the type of the specified file system.
   Some systems have statfvs.f_basetype[FSTYPSZ] (AIX, HP-UX, and Solaris).
   Others have statvfs.f_fstypename[_VFS_NAMELEN] (NetBSD 3.0).
   Others have statfs.f_fstypename[MFSNAMELEN] (NetBSD 1.5.2).
   Still others have neither and have to get by with f_type (GNU/Linux).
   But f_type may only exist in statfs (Cygwin).  */
static char const * ATTRIBUTE_WARN_UNUSED_RESULT
human_fstype (STRUCT_STATVFS const *statfsbuf)
{
  return statfsbuf->STATXFS_FILE_SYSTEM_TYPE_MEMBER_NAME;
  switch (statfsbuf->f_type)
    {

      /* Compare with what's in libc:
         f=/a/libc/sysdeps/unix/sysv/linux/linux_fsinfo.h
         sed -n '/ADFS_SUPER_MAGIC/,/SYSFS_MAGIC/p' $f \
           | perl -n -e '/#define (.*?)_(?:SUPER_)MAGIC\s+0x(\S+)/' \
             -e 'and print "case S_MAGIC_$1: /\* 0x" . uc($2) . " *\/\n"' \
           | sort > sym_libc
         perl -ne '/^\s+(case S_MAGIC_.*?): \/\* 0x(\S+) \*\//' \
             -e 'and do { $v=uc$2; print "$1: /\* 0x$v *\/\n"}' stat.c \
           | sort > sym_stat
         diff -u sym_stat sym_libc
      */

      /* Also compare with the list in "man 2 statfs" using the
         fs-magic-compare make target.  */

      /* IMPORTANT NOTE: Each of the following `case S_MAGIC_...:'
         statements must be followed by a hexadecimal constant in
         a comment.  The S_MAGIC_... name and constant are automatically
         combined to produce the #define directives in fs.h.  */

    case S_MAGIC_ADFS: /* 0xADF5 */
      return "adfs";
    case S_MAGIC_AFFS: /* 0xADFF */
      return "affs";
    case S_MAGIC_AFS: /* 0x5346414F */
      return "afs";
    case S_MAGIC_ANON_INODE_FS: /* 0x09041934 */
      return "anon-inode FS";
    case S_MAGIC_AUTOFS: /* 0x0187 */
      return "autofs";
    case S_MAGIC_BEFS: /* 0x42465331 */
      return "befs";
    case S_MAGIC_BFS: /* 0x1BADFACE */
      return "bfs";
    case S_MAGIC_BINFMT_MISC: /* 0x42494E4D */
      return "binfmt_misc";
    case S_MAGIC_BTRFS: /* 0x9123683E */
      return "btrfs";
    case S_MAGIC_CGROUP: /* 0x0027E0EB */
      return "cgroupfs";
    case S_MAGIC_CIFS: /* 0xFF534D42 */
      return "cifs";
    case S_MAGIC_CODA: /* 0x73757245 */
      return "coda";
    case S_MAGIC_COH: /* 0x012FF7B7 */
      return "coh";
    case S_MAGIC_CRAMFS: /* 0x28CD3D45 */
      return "cramfs";
    case S_MAGIC_CRAMFS_WEND: /* 0x453DCD28 */
      return "cramfs-wend";
    case S_MAGIC_DEBUGFS: /* 0x64626720 */
      return "debugfs";
    case S_MAGIC_DEVFS: /* 0x1373 */
      return "devfs";
    case S_MAGIC_DEVPTS: /* 0x1CD1 */
      return "devpts";
    case S_MAGIC_EFS: /* 0x00414A53 */
      return "efs";
    case S_MAGIC_EXT: /* 0x137D */
      return "ext";
    case S_MAGIC_EXT2: /* 0xEF53 */
      return "ext2/ext3";
    case S_MAGIC_EXT2_OLD: /* 0xEF51 */
      return "ext2";
    case S_MAGIC_FAT: /* 0x4006 */
      return "fat";
    case S_MAGIC_FUSEBLK: /* 0x65735546 */
      return "fuseblk";
    case S_MAGIC_FUSECTL: /* 0x65735543 */
      return "fusectl";
    case S_MAGIC_FUTEXFS: /* 0x0BAD1DEA */
      return "futexfs";
    case S_MAGIC_GFS: /* 0x1161970 */
      return "gfs/gfs2";
    case S_MAGIC_HFS: /* 0x4244 */
      return "hfs";
    case S_MAGIC_HPFS: /* 0xF995E849 */
      return "hpfs";
    case S_MAGIC_HUGETLBFS: /* 0x958458F6 */
      return "hugetlbfs";
    case S_MAGIC_INOTIFYFS: /* 0x2BAD1DEA */
      return "inotifyfs";
    case S_MAGIC_ISOFS: /* 0x9660 */
      return "isofs";
    case S_MAGIC_ISOFS_R_WIN: /* 0x4004 */
      return "isofs";
    case S_MAGIC_ISOFS_WIN: /* 0x4000 */
      return "isofs";
    case S_MAGIC_JFFS: /* 0x07C0 */
      return "jffs";
    case S_MAGIC_JFFS2: /* 0x72B6 */
      return "jffs2";
    case S_MAGIC_JFS: /* 0x3153464A */
      return "jfs";
    case S_MAGIC_KAFS: /* 0x6B414653 */
      return "k-afs";
    case S_MAGIC_LUSTRE: /* 0x0BD00BD0 */
      return "lustre";
    case S_MAGIC_MINIX: /* 0x137F */
      return "minix";
    case S_MAGIC_MINIX_30: /* 0x138F */
      return "minix (30 char.)";
    case S_MAGIC_MINIX_V2: /* 0x2468 */
      return "minix v2";
    case S_MAGIC_MINIX_V2_30: /* 0x2478 */
      return "minix v2 (30 char.)";
    case S_MAGIC_MINIX_V3: /* 0x4D5A */
      return "minix3";
    case S_MAGIC_MSDOS: /* 0x4D44 */
      return "msdos";
    case S_MAGIC_NCP: /* 0x564C */
      return "novell";
    case S_MAGIC_NFS: /* 0x6969 */
      return "nfs";
    case S_MAGIC_NFSD: /* 0x6E667364 */
      return "nfsd";
    case S_MAGIC_NILFS: /* 0x3434 */
      return "nilfs";
    case S_MAGIC_NTFS: /* 0x5346544E */
      return "ntfs";
    case S_MAGIC_OPENPROM: /* 0x9FA1 */
      return "openprom";
    case S_MAGIC_OCFS2: /* 0x7461636f */
      return "ocfs2";
    case S_MAGIC_PROC: /* 0x9FA0 */
      return "proc";
    case S_MAGIC_QNX4: /* 0x002F */
      return "qnx4";
    case S_MAGIC_RAMFS: /* 0x858458F6 */
      return "ramfs";
    case S_MAGIC_REISERFS: /* 0x52654973 */
      return "reiserfs";
    case S_MAGIC_ROMFS: /* 0x7275 */
      return "romfs";
    case S_MAGIC_RPC_PIPEFS: /* 0x67596969 */
      return "rpc_pipefs";
    case S_MAGIC_SECURITYFS: /* 0x73636673 */
      return "securityfs";
    case S_MAGIC_SELINUX: /* 0xF97CFF8C */
      return "selinux";
    case S_MAGIC_SMB: /* 0x517B */
      return "smb";
    case S_MAGIC_SOCKFS: /* 0x534F434B */
      return "sockfs";
    case S_MAGIC_SQUASHFS: /* 0x73717368 */
      return "squashfs";
    case S_MAGIC_SYSFS: /* 0x62656572 */
      return "sysfs";
    case S_MAGIC_SYSV2: /* 0x012FF7B6 */
      return "sysv2";
    case S_MAGIC_SYSV4: /* 0x012FF7B5 */
      return "sysv4";
    case S_MAGIC_TMPFS: /* 0x01021994 */
      return "tmpfs";
    case S_MAGIC_UDF: /* 0x15013346 */
      return "udf";
    case S_MAGIC_UFS: /* 0x00011954 */
      return "ufs";
    case S_MAGIC_UFS_BYTESWAPPED: /* 0x54190100 */
      return "ufs";
    case S_MAGIC_USBDEVFS: /* 0x9FA2 */
      return "usbdevfs";
    case S_MAGIC_VXFS: /* 0xA501FCF5 */
      return "vxfs";
    case S_MAGIC_XENFS: /* 0xABBA1974 */
      return "xenfs";
    case S_MAGIC_XENIX: /* 0x012FF7B4 */
      return "xenix";
    case S_MAGIC_XFS: /* 0x58465342 */
      return "xfs";
    case S_MAGIC_XIAFS: /* 0x012FD16D */
      return "xia";

    case FSTYPE_UFS:
      return "ufs";
    case FSTYPE_NFS:
      return "nfs";
    case FSTYPE_GFS:
      return "gfs";
    case FSTYPE_LFS:
      return "lfs";
    case FSTYPE_SYSV:
      return "sysv";
    case FSTYPE_FTP:
      return "ftp";
    case FSTYPE_TAR:
      return "tar";
    case FSTYPE_AR:
      return "ar";
    case FSTYPE_CPIO:
      return "cpio";
    case FSTYPE_MSLOSS:
      return "msloss";
    case FSTYPE_CPM:
      return "cpm";
    case FSTYPE_HFS:
      return "hfs";
    case FSTYPE_DTFS:
      return "dtfs";
    case FSTYPE_GRFS:
      return "grfs";
    case FSTYPE_TERM:
      return "term";
    case FSTYPE_DEV:
      return "dev";
    case FSTYPE_PROC:
      return "proc";
    case FSTYPE_IFSOCK:
      return "ifsock";
    case FSTYPE_AFS:
      return "afs";
    case FSTYPE_DFS:
      return "dfs";
    case FSTYPE_PROC9:
      return "proc9";
    case FSTYPE_SOCKET:
      return "socket";
    case FSTYPE_MISC:
      return "misc";
    case FSTYPE_EXT2FS:
      return "ext2/ext3";
    case FSTYPE_HTTP:
      return "http";
    case FSTYPE_MEMFS:
      return "memfs";
    case FSTYPE_ISO9660:
      return "iso9660";
    default:
      {
        unsigned long int type = statfsbuf->f_type;
        static char buf[sizeof "UNKNOWN (0x%lx)" - 3
                        + (sizeof type * CHAR_BIT + 3) / 4];
        sprintf (buf, "UNKNOWN (0x%lx)", type);
        return buf;
      }
    }
}

