void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [+FORMAT]\n\
  or:  %s [-u|--utc|--universal] [MMDDhhmm[[CC]YY][.ss]]\n\
"),
              program_name, program_name);
      fputs (_("\
Display the current time in the given FORMAT, or set the system date.\n\
\n\
  -d, --date=STRING         display time described by STRING, not `now'\n\
  -f, --file=DATEFILE       like --date once for each line of DATEFILE\n\
"), stdout);
      fputs (_("\
  -r, --reference=FILE      display the last modification time of FILE\n\
  -R, --rfc-2822            output date and time in RFC 2822 format.\n\
                            Example: Mon, 07 Aug 2006 12:34:56 -0600\n\
"), stdout);
      fputs (_("\
      --rfc-3339=TIMESPEC   output date and time in RFC 3339 format.\n\
                            TIMESPEC=`date', `seconds', or `ns' for\n\
                            date and time to the indicated precision.\n\
                            Date and time components are separated by\n\
                            a single space: 2006-08-07 12:34:56-06:00\n\
  -s, --set=STRING          set time described by STRING\n\
  -u, --utc, --universal    print or set Coordinated Universal Time\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
FORMAT controls the output.  Interpreted sequences are:\n\
\n\
  %%   a literal %\n\
  %a   locale's abbreviated weekday name (e.g., Sun)\n\
"), stdout);
      fputs (_("\
  %A   locale's full weekday name (e.g., Sunday)\n\
  %b   locale's abbreviated month name (e.g., Jan)\n\
  %B   locale's full month name (e.g., January)\n\
  %c   locale's date and time (e.g., Thu Mar  3 23:05:25 2005)\n\
"), stdout);
      fputs (_("\
  %C   century; like %Y, except omit last two digits (e.g., 20)\n\
  %d   day of month (e.g., 01)\n\
  %D   date; same as %m/%d/%y\n\
  %e   day of month, space padded; same as %_d\n\
"), stdout);
      fputs (_("\
  %F   full date; same as %Y-%m-%d\n\
  %g   last two digits of year of ISO week number (see %G)\n\
  %G   year of ISO week number (see %V); normally useful only with %V\n\
"), stdout);
      fputs (_("\
  %h   same as %b\n\
  %H   hour (00..23)\n\
  %I   hour (01..12)\n\
  %j   day of year (001..366)\n\
"), stdout);
      fputs (_("\
  %k   hour ( 0..23)\n\
  %l   hour ( 1..12)\n\
  %m   month (01..12)\n\
  %M   minute (00..59)\n\
"), stdout);
      fputs (_("\
  %n   a newline\n\
  %N   nanoseconds (000000000..999999999)\n\
  %p   locale's equivalent of either AM or PM; blank if not known\n\
  %P   like %p, but lower case\n\
  %r   locale's 12-hour clock time (e.g., 11:11:04 PM)\n\
  %R   24-hour hour and minute; same as %H:%M\n\
  %s   seconds since 1970-01-01 00:00:00 UTC\n\
"), stdout);
      fputs (_("\
  %S   second (00..60)\n\
  %t   a tab\n\
  %T   time; same as %H:%M:%S\n\
  %u   day of week (1..7); 1 is Monday\n\
"), stdout);
      fputs (_("\
  %U   week number of year, with Sunday as first day of week (00..53)\n\
  %V   ISO week number, with Monday as first day of week (01..53)\n\
  %w   day of week (0..6); 0 is Sunday\n\
  %W   week number of year, with Monday as first day of week (00..53)\n\
"), stdout);
      fputs (_("\
  %x   locale's date representation (e.g., 12/31/99)\n\
  %X   locale's time representation (e.g., 23:13:48)\n\
  %y   last two digits of year (00..99)\n\
  %Y   year\n\
"), stdout);
      fputs (_("\
  %z   +hhmm numeric time zone (e.g., -0400)\n\
  %:z  +hh:mm numeric time zone (e.g., -04:00)\n\
  %::z  +hh:mm:ss numeric time zone (e.g., -04:00:00)\n\
  %:::z  numeric time zone with : to necessary precision (e.g., -04, +05:30)\n\
  %Z   alphabetic time zone abbreviation (e.g., EDT)\n\
\n\
By default, date pads numeric fields with zeroes.\n\
"), stdout);
      fputs (_("\
The following optional flags may follow `%':\n\
\n\
  -  (hyphen) do not pad the field\n\
  _  (underscore) pad with spaces\n\
  0  (zero) pad with zeros\n\
  ^  use upper case if possible\n\
  #  use opposite case if possible\n\
"), stdout);
      fputs (_("\
\n\
After any flags comes an optional field width, as a decimal number;\n\
then an optional modifier, which is either\n\
E to use the locale's alternate representations if available, or\n\
O to use the locale's alternate numeric symbols if available.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

