/* Given these arguments describing a file, return the single-byte
   type indicator, or 0.  */
static char
get_type_indicator (bool stat_ok, mode_t mode, enum filetype type)
{
  char c;

  if (stat_ok ? S_ISREG (mode) : type == normal)
    {
      if (stat_ok && indicator_style == classify && (mode & S_IXUGO))
        c = '*';
      else
        c = 0;
    }
  else
    {
      if (stat_ok ? S_ISDIR (mode) : type == directory || type == arg_directory)
        c = '/';
      else if (indicator_style == slash)
        c = 0;
      else if (stat_ok ? S_ISLNK (mode) : type == symbolic_link)
        c = '@';
      else if (stat_ok ? S_ISFIFO (mode) : type == fifo)
        c = '|';
      else if (stat_ok ? S_ISSOCK (mode) : type == sock)
        c = '=';
      else if (stat_ok && S_ISDOOR (mode))
        c = '>';
      else
        c = 0;
    }
  return c;
}

