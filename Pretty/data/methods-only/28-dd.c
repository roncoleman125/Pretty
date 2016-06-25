/* Copy NREAD bytes of BUF, doing conv=unblock
   (replace trailing spaces in `conversion_blocksize'-sized records
   with a newline).  */

static void
copy_with_unblock (char const *buf, size_t nread)
{
  size_t i;
  char c;
  static size_t pending_spaces = 0;

  for (i = 0; i < nread; i++)
    {
      c = buf[i];

      if (col++ >= conversion_blocksize)
        {
          col = pending_spaces = 0; /* Wipe out any pending spaces.  */
          i--;			/* Push the char back; get it later. */
          output_char (newline_character);
        }
      else if (c == space_character)
        pending_spaces++;
      else
        {
          /* `c' is the character after a run of spaces that were not
             at the end of the conversion buffer.  Output them.  */
          while (pending_spaces)
            {
              output_char (space_character);
              --pending_spaces;
            }
          output_char (c);
        }
    }
}

