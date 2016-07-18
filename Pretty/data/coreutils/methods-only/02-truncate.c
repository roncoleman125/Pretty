/* return true on success, false on error.  */
static bool
do_ftruncate (int fd, char const *fname, off_t ssize, off_t rsize,
              rel_mode_t rel_mode)
{
  struct stat sb;
  off_t nsize;

  if ((block_mode || (rel_mode && rsize < 0)) && fstat (fd, &sb) != 0)
    {
      error (0, errno, _("cannot fstat %s"), quote (fname));
      return false;
    }
  if (block_mode)
    {
      off_t const blksize = ST_BLKSIZE (sb);
      if (ssize < OFF_T_MIN / blksize || ssize > OFF_T_MAX / blksize)
        {
          error (0, 0,
                 _("overflow in %" PRIdMAX
                   " * %" PRIdMAX " byte blocks for file %s"),
                 (intmax_t) ssize, (intmax_t) blksize,
                 quote (fname));
          return false;
        }
      ssize *= blksize;
    }
  if (rel_mode)
    {
      uintmax_t const fsize = rsize < 0 ? sb.st_size : rsize;

      if (rsize < 0) /* fstat used above to get size.  */
        {
          if (!S_ISREG (sb.st_mode) && !S_TYPEISSHM (&sb))
            {
              error (0, 0, _("cannot get the size of %s"), quote (fname));
              return false;
            }
          if (sb.st_size < 0)
            {
              /* Sanity check. Overflow is the only reason I can think
                 this would ever go negative. */
              error (0, 0, _("%s has unusable, apparently negative size"),
                     quote (fname));
              return false;
            }
        }

      if (rel_mode == rm_min)
        nsize = MAX (fsize, (uintmax_t) ssize);
      else if (rel_mode == rm_max)
        nsize = MIN (fsize, (uintmax_t) ssize);
      else if (rel_mode == rm_rdn)
        /* 0..ssize-1 -> 0 */
        nsize = (fsize / ssize) * ssize;
      else if (rel_mode == rm_rup)
        /* 1..ssize -> ssize */
        {
          /* Here ssize>=1 && fsize>=0 */
          uintmax_t const overflow = ((fsize + ssize - 1) / ssize) * ssize;
          if (overflow > OFF_T_MAX)
            {
              error (0, 0, _("overflow rounding up size of file %s"),
                     quote (fname));
              return false;
            }
          nsize = overflow;
        }
      else
        {
          if (ssize > OFF_T_MAX - (off_t)fsize)
            {
              error (0, 0, _("overflow extending size of file %s"),
                     quote (fname));
              return false;
            }
          nsize = fsize + ssize;
        }
    }
  else
    nsize = ssize;
  if (nsize < 0)
    nsize = 0;

  if (ftruncate (fd, nsize) == -1)      /* note updates mtime & ctime */
    {
      error (0, errno,
             _("failed to truncate %s at %" PRIdMAX " bytes"), quote (fname),
             (intmax_t) nsize);
      return false;
    }

  return true;
}

