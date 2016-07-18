/* Print in reverse the file open on descriptor FD for reading FILE.
   Return true if successful.  */

static bool
tac_seekable (int input_fd, const char *file)
{
  /* Pointer to the location in `G_buffer' where the search for
     the next separator will begin. */
  char *match_start;

  /* Pointer to one past the rightmost character in `G_buffer' that
     has not been printed yet. */
  char *past_end;

  /* Length of the record growing in `G_buffer'. */
  size_t saved_record_size;

  /* Offset in the file of the next read. */
  off_t file_pos;

  /* True if `output' has not been called yet for any file.
     Only used when the separator is attached to the preceding record. */
  bool first_time = true;
  char first_char = *separator;	/* Speed optimization, non-regexp. */
  char const *separator1 = separator + 1; /* Speed optimization, non-regexp. */
  size_t match_length1 = match_length - 1; /* Speed optimization, non-regexp. */

  /* Find the size of the input file. */
  file_pos = lseek (input_fd, (off_t) 0, SEEK_END);
  if (file_pos < 1)
    return true;			/* It's an empty file. */

  /* Arrange for the first read to lop off enough to leave the rest of the
     file a multiple of `read_size'.  Since `read_size' can change, this may
     not always hold during the program run, but since it usually will, leave
     it here for i/o efficiency (page/sector boundaries and all that).
     Note: the efficiency gain has not been verified. */
  saved_record_size = file_pos % read_size;
  if (saved_record_size == 0)
    saved_record_size = read_size;
  file_pos -= saved_record_size;
  /* `file_pos' now points to the start of the last (probably partial) block
     in the input file. */

  if (lseek (input_fd, file_pos, SEEK_SET) < 0)
    error (0, errno, _("%s: seek failed"), quotearg_colon (file));

  if (safe_read (input_fd, G_buffer, saved_record_size) != saved_record_size)
    {
      error (0, errno, _("%s: read error"), quotearg_colon (file));
      return false;
    }

  match_start = past_end = G_buffer + saved_record_size;
  /* For non-regexp search, move past impossible positions for a match. */
  if (sentinel_length)
    match_start -= match_length1;

  while (true)
    {
      /* Search backward from `match_start' - 1 to `G_buffer' for a match
         with `separator'; for speed, use strncmp if `separator' contains no
         metacharacters.
         If the match succeeds, set `match_start' to point to the start of
         the match and `match_length' to the length of the match.
         Otherwise, make `match_start' < `G_buffer'. */
      if (sentinel_length == 0)
        {
          size_t i = match_start - G_buffer;
          regoff_t ri = i;
          regoff_t range = 1 - ri;
          regoff_t ret;

          if (1 < range)
            error (EXIT_FAILURE, 0, _("record too large"));

          if (range == 1
              || ((ret = re_search (&compiled_separator, G_buffer,
                                    i, i - 1, range, &regs))
                  == -1))
            match_start = G_buffer - 1;
          else if (ret == -2)
            {
              error (EXIT_FAILURE, 0,
                     _("error in regular expression search"));
            }
          else
            {
              match_start = G_buffer + regs.start[0];
              match_length = regs.end[0] - regs.start[0];
            }
        }
      else
        {
          /* `match_length' is constant for non-regexp boundaries. */
          while (*--match_start != first_char
                 || (match_length1 && strncmp (match_start + 1, separator1,
                                               match_length1)))
            /* Do nothing. */ ;
        }

      /* Check whether we backed off the front of `G_buffer' without finding
         a match for `separator'. */
      if (match_start < G_buffer)
        {
          if (file_pos == 0)
            {
              /* Hit the beginning of the file; print the remaining record. */
              output (G_buffer, past_end);
              return true;
            }

          saved_record_size = past_end - G_buffer;
          if (saved_record_size > read_size)
            {
              /* `G_buffer_size' is about twice `read_size', so since
                 we want to read in another `read_size' bytes before
                 the data already in `G_buffer', we need to increase
                 `G_buffer_size'. */
              char *newbuffer;
              size_t offset = sentinel_length ? sentinel_length : 1;
              ptrdiff_t match_start_offset = match_start - G_buffer;
              ptrdiff_t past_end_offset = past_end - G_buffer;
              size_t old_G_buffer_size = G_buffer_size;

              read_size *= 2;
              G_buffer_size = read_size * 2 + sentinel_length + 2;
              if (G_buffer_size < old_G_buffer_size)
                xalloc_die ();
              newbuffer = xrealloc (G_buffer - offset, G_buffer_size);
              newbuffer += offset;
              /* Adjust the pointers for the new buffer location.  */
              match_start = newbuffer + match_start_offset;
              past_end = newbuffer + past_end_offset;
              G_buffer = newbuffer;
            }

          /* Back up to the start of the next bufferfull of the file.  */
          if (file_pos >= read_size)
            file_pos -= read_size;
          else
            {
              read_size = file_pos;
              file_pos = 0;
            }
          if (lseek (input_fd, file_pos, SEEK_SET) < 0)
            error (0, errno, _("%s: seek failed"), quotearg_colon (file));

          /* Shift the pending record data right to make room for the new.
             The source and destination regions probably overlap.  */
          memmove (G_buffer + read_size, G_buffer, saved_record_size);
          past_end = G_buffer + read_size + saved_record_size;
          /* For non-regexp searches, avoid unneccessary scanning. */
          if (sentinel_length)
            match_start = G_buffer + read_size;
          else
            match_start = past_end;

          if (safe_read (input_fd, G_buffer, read_size) != read_size)
            {
              error (0, errno, _("%s: read error"), quotearg_colon (file));
              return false;
            }
        }
      else
        {
          /* Found a match of `separator'. */
          if (separator_ends_record)
            {
              char *match_end = match_start + match_length;

              /* If this match of `separator' isn't at the end of the
                 file, print the record. */
              if (!first_time || match_end != past_end)
                output (match_end, past_end);
              past_end = match_end;
              first_time = false;
            }
          else
            {
              output (match_start, past_end);
              past_end = match_start;
            }

          /* For non-regex matching, we can back up.  */
          if (sentinel_length > 0)
            match_start -= match_length - 1;
        }
    }
}


/* FIXME-someday: remove all of this DONT_UNLINK_WHILE_OPEN junk.
   Using atexit like this is wrong, since it can fail
   when called e.g. 32 or more times.
   But this isn't a big deal, since the code is used only on WOE/DOS
   systems, and few people invoke tac on that many nonseekable files.  */

static const char *file_to_remove;
static FILE *fp_to_close;

static void
unlink_tempfile (void)
{
  fclose (fp_to_close);
  unlink (file_to_remove);
}

static void
record_or_unlink_tempfile (char const *fn, FILE *fp)
{
  if (!file_to_remove)
    {
      file_to_remove = fn;
      fp_to_close = fp;
      atexit (unlink_tempfile);
    }
}


static void
record_or_unlink_tempfile (char const *fn, FILE *fp ATTRIBUTE_UNUSED)
{
  unlink (fn);
}


