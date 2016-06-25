void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... LAST\n\
  or:  %s [OPTION]... FIRST LAST\n\
  or:  %s [OPTION]... FIRST INCREMENT LAST\n\
"), program_name, program_name, program_name);
      fputs (_("\
Print numbers from FIRST to LAST, in steps of INCREMENT.\n\
\n\
  -f, --format=FORMAT      use printf style floating-point FORMAT\n\
  -s, --separator=STRING   use STRING to separate numbers (default: \\n)\n\
  -w, --equal-width        equalize width by padding with leading zeroes\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
If FIRST or INCREMENT is omitted, it defaults to 1.  That is, an\n\
omitted INCREMENT defaults to 1 even when LAST is smaller than FIRST.\n\
FIRST, INCREMENT, and LAST are interpreted as floating point values.\n\
INCREMENT is usually positive if FIRST is smaller than LAST, and\n\
INCREMENT is usually negative if FIRST is greater than LAST.\n\
"), stdout);
      fputs (_("\
FORMAT must be suitable for printing one argument of type `double';\n\
it defaults to %.PRECf if FIRST, INCREMENT, and LAST are all fixed point\n\
decimal numbers with maximum precision PREC, and to %g otherwise.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

/* A command-line operand.  */
struct operand
{
  /* Its value, converted to 'long double'.  */
  long double value;

  /* Its print width, if it were printed out in a form similar to its
     input form.  An input like "-.1" is treated like "-0.1", and an
     input like "1." is treated like "1", but otherwise widths are
     left alone.  */
  size_t width;

  /* Number of digits after the decimal point, or INT_MAX if the
     number can't easily be expressed as a fixed-point number.  */
  int precision;
};
typedef struct operand operand;

/* Description of what a number-generating format will generate.  */
struct layout
{
  /* Number of bytes before and after the number.  */
  size_t prefix_len;
  size_t suffix_len;
};

