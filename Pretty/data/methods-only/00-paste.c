/* Set globals delims and delim_end.  Copy STRPTR to DELIMS, converting
   backslash representations of special characters in STRPTR to their actual
   values. The set of possible backslash characters has been expanded beyond
   that recognized by the Unix version.
   Return 0 upon success.
   If the string ends in an odd number of backslashes, ignore the
   final backslash and return nonzero.  */

static int
collapse_escapes (char const *strptr)
{
  char *strout = xstrdup (strptr);
  bool backslash_at_end = false;

  delims = strout;

  while (*strptr)
    {
      if (*strptr != '\\')	/* Is it an escape character? */
        *strout++ = *strptr++;	/* No, just transfer it. */
      else
        {
          switch (*++strptr)
            {
            case '0':
              *strout++ = EMPTY_DELIM;
              break;

            case 'b':
              *strout++ = '\b';
              break;

            case 'f':
              *strout++ = '\f';
              break;

            case 'n':
              *strout++ = '\n';
              break;

            case 'r':
              *strout++ = '\r';
              break;

            case 't':
              *strout++ = '\t';
              break;

            case 'v':
              *strout++ = '\v';
              break;

            case '\\':
              *strout++ = '\\';
              break;

            case '\0':
              backslash_at_end = true;
              goto done;

            default:
              *strout++ = *strptr;
              break;
            }
          strptr++;
        }
    }

 done:;

  delim_end = strout;
  return backslash_at_end ? 1 : 0;
}

/* Report a write error and exit.  */

static void write_error (void) ATTRIBUTE_NORETURN;
