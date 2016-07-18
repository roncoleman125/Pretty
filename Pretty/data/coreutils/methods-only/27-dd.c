/* Copy NREAD bytes of BUF, doing conv=block
   (pad newline-terminated records to `conversion_blocksize',
   replacing the newline with trailing spaces).  */

static void
copy_with_block (char const *buf, size_t nread)
{
  size_t i;

  for (i = nread; i; i--, buf++)
    {
      if (*buf == newline_character)
        {
          if (col < conversion_blocksize)
            {
              size_t j;
              for (j = col; j < conversion_blocksize; j++)
                output_char (space_character);
            }
          col = 0;
        }
      else
        {
          if (col == conversion_blocksize)
            r_truncate++;
          else if (col < conversion_blocksize)
            output_char (*buf);
          col++;
        }
    }
}

