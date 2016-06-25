/* Add OFFSET to the input offset, setting the overflow flag if
   necessary.  */

static void
advance_input_offset (uintmax_t offset)
{
  input_offset += offset;
  if (input_offset < offset)
    input_offset_overflow = true;
}

/* This is a wrapper for lseek.  It detects and warns about a kernel
   bug that makes lseek a no-op for tape devices, even though the kernel
   lseek return value suggests that the function succeeded.

   The parameters are the same as those of the lseek function, but
   with the addition of FILENAME, the name of the file associated with
   descriptor FDESC.  The file name is used solely in the warning that's
   printed when the bug is detected.  Return the same value that lseek
   would have returned, but when the lseek bug is detected, return -1
   to indicate that lseek failed.

   The offending behavior has been confirmed with an Exabyte SCSI tape
   drive accessed via /dev/nst0 on both Linux 2.2.17 and 2.4.16 kernels.  */



   ((P).mt_resid == (Q).mt_resid \
    && (P).mt_fileno == (Q).mt_fileno \
    && (P).mt_blkno == (Q).mt_blkno)

static off_t
skip_via_lseek (char const *filename, int fdesc, off_t offset, int whence)
{
  struct mtget s1;
  struct mtget s2;
  bool got_original_tape_position = (ioctl (fdesc, MTIOCGET, &s1) == 0);
  /* known bad device type */
  /* && s.mt_type == MT_ISSCSI2 */

  off_t new_position = lseek (fdesc, offset, whence);
  if (0 <= new_position
      && got_original_tape_position
      && ioctl (fdesc, MTIOCGET, &s2) == 0
      && MT_SAME_POSITION (s1, s2))
    {
      error (0, 0, _("warning: working around lseek kernel bug for file (%s)\n\
  of mt_type=0x%0lx -- see <sys/mtio.h> for the list of types"),
             filename, s2.mt_type);
      errno = 0;
      new_position = -1;
    }

  return new_position;
}

