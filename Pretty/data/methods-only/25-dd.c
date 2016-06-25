/* Advance the input by NBYTES if possible, after a read error.
   The input file offset may or may not have advanced after the failed
   read; adjust it to point just after the bad record regardless.
   Return true if successful, or if the input is already known to not
   be seekable.  */

static bool
advance_input_after_read_error (size_t nbytes)
{
  if (! input_seekable)
    {
      if (input_seek_errno == ESPIPE)
        return true;
      errno = input_seek_errno;
    }
  else
    {
      off_t offset;
      advance_input_offset (nbytes);
      input_offset_overflow |= (OFF_T_MAX < input_offset);
      if (input_offset_overflow)
        {
          error (0, 0, _("offset overflow while reading file %s"),
                 quote (input_file));
          return false;
        }
      offset = lseek (STDIN_FILENO, 0, SEEK_CUR);
      if (0 <= offset)
        {
          off_t diff;
          if (offset == input_offset)
            return true;
          diff = input_offset - offset;
          if (! (0 <= diff && diff <= nbytes))
            error (0, 0, _("warning: invalid file offset after failed read"));
          if (0 <= skip_via_lseek (input_file, STDIN_FILENO, diff, SEEK_CUR))
            return true;
          if (errno == 0)
            error (0, 0, _("cannot work around kernel bug after all"));
        }
    }

  error (0, errno, _("%s: cannot seek"), quote (input_file));
  return false;
}

