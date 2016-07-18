/* Count words.  FILE_X is the name of the file (or NULL for standard
   input) that is open on descriptor FD.  *FSTATUS is its status.
   Return true if successful.  */
static bool
wc (int fd, char const *file_x, struct fstatus *fstatus)
{
  bool ok = true;
  char buf[BUFFER_SIZE + 1];
  size_t bytes_read;
  uintmax_t lines, words, chars, bytes, linelength;
  bool count_bytes, count_chars, count_complicated;
  char const *file = file_x ? file_x : _("standard input");

  lines = words = chars = bytes = linelength = 0;

  /* If in the current locale, chars are equivalent to bytes, we prefer
     counting bytes, because that's easier.  */
  if (MB_CUR_MAX > 1)
    {
      count_bytes = print_bytes;
      count_chars = print_chars;
    }
  else
    {
      count_bytes = print_bytes || print_chars;
      count_chars = false;
    }
  count_complicated = print_words || print_linelength;

  /* Advise the kernel of our access pattern only if we will read().  */
  if (!count_bytes || count_chars || print_lines || count_complicated)
    fdadvise (fd, 0, 0, FADVISE_SEQUENTIAL);

  /* When counting only bytes, save some line- and word-counting
     overhead.  If FD is a `regular' Unix file, using lseek is enough
     to get its `size' in bytes.  Otherwise, read blocks of BUFFER_SIZE
     bytes at a time until EOF.  Note that the `size' (number of bytes)
     that wc reports is smaller than stats.st_size when the file is not
     positioned at its beginning.  That's why the lseek calls below are
     necessary.  For example the command
     `(dd ibs=99k skip=1 count=0; ./wc -c) < /etc/group'
     should make wc report `0' bytes.  */

  if (count_bytes && !count_chars && !print_lines && !count_complicated)
    {
      off_t current_pos, end_pos;

      if (0 < fstatus->failed)
        fstatus->failed = fstat (fd, &fstatus->st);

      if (! fstatus->failed && S_ISREG (fstatus->st.st_mode)
          && (current_pos = lseek (fd, (off_t) 0, SEEK_CUR)) != -1
          && (end_pos = lseek (fd, (off_t) 0, SEEK_END)) != -1)
        {
          /* Be careful here.  The current position may actually be
             beyond the end of the file.  As in the example above.  */
          bytes = end_pos < current_pos ? 0 : end_pos - current_pos;
        }
      else
        {
          fdadvise (fd, 0, 0, FADVISE_SEQUENTIAL);
          while ((bytes_read = safe_read (fd, buf, BUFFER_SIZE)) > 0)
            {
              if (bytes_read == SAFE_READ_ERROR)
                {
                  error (0, errno, "%s", file);
                  ok = false;
                  break;
                }
              bytes += bytes_read;
            }
        }
    }
  else if (!count_chars && !count_complicated)
    {
      /* Use a separate loop when counting only lines or lines and bytes --
         but not chars or words.  */
      while ((bytes_read = safe_read (fd, buf, BUFFER_SIZE)) > 0)
        {
          char *p = buf;

          if (bytes_read == SAFE_READ_ERROR)
            {
              error (0, errno, "%s", file);
              ok = false;
              break;
            }

          while ((p = memchr (p, '\n', (buf + bytes_read) - p)))
            {
              ++p;
              ++lines;
            }
          bytes += bytes_read;
        }
    }
  else if (MB_CUR_MAX > 1)
    {
      bool in_word = false;
      uintmax_t linepos = 0;
      DECLARE_ZEROED_AGGREGATE (mbstate_t, state);
      bool in_shift = false;
      /* Back-up the state before each multibyte character conversion and
         move the last incomplete character of the buffer to the front
         of the buffer.  This is needed because we don't know whether
         the `mbrtowc' function updates the state when it returns -2, -
         this is the ISO C 99 and glibc-2.2 behaviour - or not - amended
         ANSI C, glibc-2.1 and Solaris 5.7 behaviour.  We don't have an
         autoconf test for this, yet.  */
      size_t prev = 0; /* number of bytes carried over from previous round */
      const size_t prev = 0;

      while ((bytes_read = safe_read (fd, buf + prev, BUFFER_SIZE - prev)) > 0)
        {
          const char *p;
          mbstate_t backup_state;
          if (bytes_read == SAFE_READ_ERROR)
            {
              error (0, errno, "%s", file);
              ok = false;
              break;
            }

          bytes += bytes_read;
          p = buf;
          bytes_read += prev;
          do
            {
              wchar_t wide_char;
              size_t n;

              if (!in_shift && is_basic (*p))
                {
                  /* Handle most ASCII characters quickly, without calling
                     mbrtowc().  */
                  n = 1;
                  wide_char = *p;
                }
              else
                {
                  in_shift = true;
                  backup_state = state;
                  n = mbrtowc (&wide_char, p, bytes_read, &state);
                  if (n == (size_t) -2)
                    {
                      state = backup_state;
                      break;
                    }
                  if (n == (size_t) -1)
                    {
                      /* Remember that we read a byte, but don't complain
                         about the error.  Because of the decoding error,
                         this is a considered to be byte but not a
                         character (that is, chars is not incremented).  */
                      p++;
                      bytes_read--;
                      continue;
                    }
                  if (mbsinit (&state))
                    in_shift = false;
                  if (n == 0)
                    {
                      wide_char = 0;
                      n = 1;
                    }
                }
              p += n;
              bytes_read -= n;
              chars++;
              switch (wide_char)
                {
                case '\n':
                  lines++;
                  /* Fall through. */
                case '\r':
                case '\f':
                  if (linepos > linelength)
                    linelength = linepos;
                  linepos = 0;
                  goto mb_word_separator;
                case '\t':
                  linepos += 8 - (linepos % 8);
                  goto mb_word_separator;
                case ' ':
                  linepos++;
                  /* Fall through. */
                case '\v':
                mb_word_separator:
                  words += in_word;
                  in_word = false;
                  break;
                default:
                  if (iswprint (wide_char))
                    {
                      int width = wcwidth (wide_char);
                      if (width > 0)
                        linepos += width;
                      if (iswspace (wide_char))
                        goto mb_word_separator;
                      in_word = true;
                    }
                  break;
                }
            }
          while (bytes_read > 0);

          if (bytes_read > 0)
            {
              if (bytes_read == BUFFER_SIZE)
                {
                  /* Encountered a very long redundant shift sequence.  */
                  p++;
                  bytes_read--;
                }
              memmove (buf, p, bytes_read);
            }
          prev = bytes_read;
        }
      if (linepos > linelength)
        linelength = linepos;
      words += in_word;
    }
  else
    {
      bool in_word = false;
      uintmax_t linepos = 0;

      while ((bytes_read = safe_read (fd, buf, BUFFER_SIZE)) > 0)
        {
          const char *p = buf;
          if (bytes_read == SAFE_READ_ERROR)
            {
              error (0, errno, "%s", file);
              ok = false;
              break;
            }

          bytes += bytes_read;
          do
            {
              switch (*p++)
                {
                case '\n':
                  lines++;
                  /* Fall through. */
                case '\r':
                case '\f':
                  if (linepos > linelength)
                    linelength = linepos;
                  linepos = 0;
                  goto word_separator;
                case '\t':
                  linepos += 8 - (linepos % 8);
                  goto word_separator;
                case ' ':
                  linepos++;
                  /* Fall through. */
                case '\v':
                word_separator:
                  words += in_word;
                  in_word = false;
                  break;
                default:
                  if (isprint (to_uchar (p[-1])))
                    {
                      linepos++;
                      if (isspace (to_uchar (p[-1])))
                        goto word_separator;
                      in_word = true;
                    }
                  break;
                }
            }
          while (--bytes_read);
        }
      if (linepos > linelength)
        linelength = linepos;
      words += in_word;
    }

  if (count_chars < print_chars)
    chars = bytes;

  write_counts (lines, words, chars, bytes, linelength, file_x);
  total_lines += lines;
  total_words += words;
  total_chars += chars;
  total_bytes += bytes;
  if (linelength > max_line_length)
    max_line_length = linelength;

  return ok;
}

