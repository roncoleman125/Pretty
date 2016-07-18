/* Scan the printf format flags in FORMAT, storing info about the
   flags into *FLAGS_PTR.  Return the number of flags found.  */
static size_t
get_format_flags (char const *format, int *flags_ptr)
{
  int flags = 0;

  for (size_t count = 0; ; count++)
    {
      switch (format[count])
        {
        case '-':
        case '0':
          break;

        case '\'':
          flags |= FLAG_THOUSANDS;
          break;

        case '#':
          flags |= FLAG_ALTERNATIVE;
          break;

        default:
          *flags_ptr = flags;
          return count;
        }
    }
}

