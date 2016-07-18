/* Return an allocated format string in static storage that
   corresponds to whether FS and TERSE options were declared.  */
static char *
default_format (bool fs, bool terse, bool device)
{
  char *format;
  if (fs)
    {
      if (terse)
        format = xstrdup ("%n %i %l %t %s %S %b %f %a %c %d\n");
      else
        {
          /* TRANSLATORS: This string uses format specifiers from
             'stat --help' with --file-system, and NOT from printf.  */
          format = xstrdup (_("\
  File: \"%n\"\n\
    ID: %-8i Namelen: %-7l Type: %T\n\
Block size: %-10s Fundamental block size: %S\n\
Blocks: Total: %-10b Free: %-10f Available: %a\n\
Inodes: Total: %-10c Free: %d\n\
"));
        }
    }
  else /* ! fs */
    {
      if (terse)
        {
          if (0 < is_selinux_enabled ())
            format = xstrdup ("%n %s %b %f %u %g %D %i %h %t %T"
                              " %X %Y %Z %W %o %C\n");
          else
            format = xstrdup ("%n %s %b %f %u %g %D %i %h %t %T"
                              " %X %Y %Z %W %o\n");
        }
      else
        {
          char *temp;
          /* TRANSLATORS: This string uses format specifiers from
             'stat --help' without --file-system, and NOT from printf.  */
          format = xstrdup (_("\
  File: %N\n\
  Size: %-10s\tBlocks: %-10b IO Block: %-6o %F\n\
"));

          temp = format;
          if (device)
            {
              /* TRANSLATORS: This string uses format specifiers from
                 'stat --help' without --file-system, and NOT from printf.  */
              format = xasprintf ("%s%s", format, _("\
Device: %Dh/%dd\tInode: %-10i  Links: %-5h Device type: %t,%T\n\
"));
            }
          else
            {
              /* TRANSLATORS: This string uses format specifiers from
                 'stat --help' without --file-system, and NOT from printf.  */
              format = xasprintf ("%s%s", format, _("\
Device: %Dh/%dd\tInode: %-10i  Links: %h\n\
"));
            }
          free (temp);

          temp = format;
          /* TRANSLATORS: This string uses format specifiers from
             'stat --help' without --file-system, and NOT from printf.  */
          format = xasprintf ("%s%s", format, _("\
Access: (%04a/%10.10A)  Uid: (%5u/%8U)   Gid: (%5g/%8G)\n\
"));
          free (temp);

          if (0 < is_selinux_enabled ())
            {
              temp = format;
              /* TRANSLATORS: This string uses format specifiers from
                 'stat --help' without --file-system, and NOT from printf.  */
              format = xasprintf ("%s%s", format, _("\
Context: %C\n\
"));
              free (temp);
            }

          temp = format;
          /* TRANSLATORS: This string uses format specifiers from
             'stat --help' without --file-system, and NOT from printf.  */
          format = xasprintf ("%s%s", format, _("\
Access: %x\n\
Modify: %y\n\
Change: %z\n\
 Birth: %w\n\
"));
          free (temp);
        }
    }
  return format;
}

