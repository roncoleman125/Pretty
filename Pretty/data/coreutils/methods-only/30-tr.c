/* Read buffers of SIZE bytes via the function READER (if READER is
   NULL, read from stdin) until EOF.  When non-NULL, READER is either
   read_and_delete or read_and_xlate.  After each buffer is read, it is
   processed and written to stdout.  The buffers are processed so that
   multiple consecutive occurrences of the same character in the input
   stream are replaced by a single occurrence of that character if the
   character is in the squeeze set.  */

static void
squeeze_filter (char *buf, size_t size, size_t (*reader) (char *, size_t))
{
  /* A value distinct from any character that may have been stored in a
     buffer as the result of a block-read in the function squeeze_filter.  */
  enum { NOT_A_CHAR = CHAR_MAX + 1 };

  int char_to_squeeze = NOT_A_CHAR;
  size_t i = 0;
  size_t nr = 0;

  while (true)
    {
      size_t begin;

      if (i >= nr)
        {
          nr = reader (buf, size);
          if (nr == 0)
            break;
          i = 0;
        }

      begin = i;

      if (char_to_squeeze == NOT_A_CHAR)
        {
          size_t out_len;
          /* Here, by being a little tricky, we can get a significant
             performance increase in most cases when the input is
             reasonably large.  Since tr will modify the input only
             if two consecutive (and identical) input characters are
             in the squeeze set, we can step by two through the data
             when searching for a character in the squeeze set.  This
             means there may be a little more work in a few cases and
             perhaps twice as much work in the worst cases where most
             of the input is removed by squeezing repeats.  But most
             uses of this functionality seem to remove less than 20-30%
             of the input.  */
          for (; i < nr && !in_squeeze_set[to_uchar (buf[i])]; i += 2)
            continue;

          /* There is a special case when i == nr and we've just
             skipped a character (the last one in buf) that is in
             the squeeze set.  */
          if (i == nr && in_squeeze_set[to_uchar (buf[i - 1])])
            --i;

          if (i >= nr)
            out_len = nr - begin;
          else
            {
              char_to_squeeze = buf[i];
              /* We're about to output buf[begin..i].  */
              out_len = i - begin + 1;

              /* But since we stepped by 2 in the loop above,
                 out_len may be one too large.  */
              if (i > 0 && buf[i - 1] == char_to_squeeze)
                --out_len;

              /* Advance i to the index of first character to be
                 considered when looking for a char different from
                 char_to_squeeze.  */
              ++i;
            }
          if (out_len > 0
              && fwrite (&buf[begin], 1, out_len, stdout) != out_len)
            error (EXIT_FAILURE, errno, _("write error"));
        }

      if (char_to_squeeze != NOT_A_CHAR)
        {
          /* Advance i to index of first char != char_to_squeeze
             (or to nr if all the rest of the characters in this
             buffer are the same as char_to_squeeze).  */
          for (; i < nr && buf[i] == char_to_squeeze; i++)
            continue;
          if (i < nr)
            char_to_squeeze = NOT_A_CHAR;
          /* If (i >= nr) we've squeezed the last character in this buffer.
             So now we have to read a new buffer and continue comparing
             characters against char_to_squeeze.  */
        }
    }
}

