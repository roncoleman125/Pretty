void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [FILE]...\n\
  or:  %s [-abcdfilosx]... [FILE] [[+]OFFSET[.][b]]\n\
  or:  %s --traditional [OPTION]... [FILE] [[+]OFFSET[.][b] [+][LABEL][.][b]]\n\
"),
              program_name, program_name, program_name);
      fputs (_("\n\
Write an unambiguous representation, octal bytes by default,\n\
of FILE to standard output.  With more than one FILE argument,\n\
concatenate them in the listed order to form the input.\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
"), stdout);
      fputs (_("\
All arguments to long options are mandatory for short options.\n\
"), stdout);
      fputs (_("\
  -A, --address-radix=RADIX   decide how file offsets are printed\n\
  -j, --skip-bytes=BYTES      skip BYTES input bytes first\n\
"), stdout);
      fputs (_("\
  -N, --read-bytes=BYTES      limit dump to BYTES input bytes\n\
  -S, --strings[=BYTES]       output strings of at least BYTES graphic chars\n\
  -t, --format=TYPE           select output format or formats\n\
  -v, --output-duplicates     do not use * to mark line suppression\n\
  -w, --width[=BYTES]         output BYTES bytes per output line\n\
      --traditional           accept arguments in traditional form\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Traditional format specifications may be intermixed; they accumulate:\n\
  -a   same as -t a,  select named characters, ignoring high-order bit\n\
  -b   same as -t o1, select octal bytes\n\
  -c   same as -t c,  select ASCII characters or backslash escapes\n\
  -d   same as -t u2, select unsigned decimal 2-byte units\n\
"), stdout);
      fputs (_("\
  -f   same as -t fF, select floats\n\
  -i   same as -t dI, select decimal ints\n\
  -l   same as -t dL, select decimal longs\n\
  -o   same as -t o2, select octal 2-byte units\n\
  -s   same as -t d2, select decimal 2-byte units\n\
  -x   same as -t x2, select hexadecimal 2-byte units\n\
"), stdout);
      fputs (_("\
\n\
If first and second call formats both apply, the second format is assumed\n\
if the last operand begins with + or (if there are 2 operands) a digit.\n\
An OFFSET operand means -j OFFSET.  LABEL is the pseudo-address\n\
at first byte printed, incremented when dump is progressing.\n\
For OFFSET and LABEL, a 0x or 0X prefix indicates hexadecimal;\n\
suffixes may be . for octal and b for multiply by 512.\n\
"), stdout);
      fputs (_("\
\n\
TYPE is made up of one or more of these specifications:\n\
\n\
  a          named character, ignoring high-order bit\n\
  c          ASCII character or backslash escape\n\
"), stdout);
      fputs (_("\
  d[SIZE]    signed decimal, SIZE bytes per integer\n\
  f[SIZE]    floating point, SIZE bytes per integer\n\
  o[SIZE]    octal, SIZE bytes per integer\n\
  u[SIZE]    unsigned decimal, SIZE bytes per integer\n\
  x[SIZE]    hexadecimal, SIZE bytes per integer\n\
"), stdout);
      fputs (_("\
\n\
SIZE is a number.  For TYPE in doux, SIZE may also be C for\n\
sizeof(char), S for sizeof(short), I for sizeof(int) or L for\n\
sizeof(long).  If TYPE is f, SIZE may also be F for sizeof(float), D\n\
for sizeof(double) or L for sizeof(long double).\n\
"), stdout);
      fputs (_("\
\n\
RADIX is d for decimal, o for octal, x for hexadecimal or n for none.\n\
BYTES is hexadecimal with 0x or 0X prefix, and may have a multiplier suffix:\n\
b 512, kB 1000, K 1024, MB 1000*1000, M 1024*1024,\n\
GB 1000*1000*1000, G 1024*1024*1024, and so on for T, P, E, Z, Y.\n\
Adding a z suffix to any type displays printable characters at the end of each\
\n\
output line.\n\
"), stdout);
      fputs (_("\
Option --string without a number implies 3; option --width without a number\n\
implies 32.  By default, od uses -A o -t oS -w16.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

/* Define the print functions.  */

static void                                                             \
N (size_t fields, size_t blank, void const *block,                      \
   char const *FMT_STRING, int width, int pad)                          \
{                                                                       \
  T const *p = block;                                                   \
  size_t i;                                                             \
  int pad_remaining = pad;                                              \
  for (i = fields; blank < i; i--)                                      \
    {                                                                   \
      int next_pad = pad * (i - 1) / fields;                            \
      int adjusted_width = pad_remaining - next_pad + width;            \
      T x = *p++;                                                       \
      ACTION;                                                           \
      pad_remaining = next_pad;                                         \
    }                                                                   \
}

  PRINT_FIELDS (N, T, fmt_string, xprintf (fmt_string, adjusted_width, x))

  PRINT_FIELDS (N, T, fmt_string ATTRIBUTE_UNUSED,                      \
                char buf[BUFSIZE];                                      \
                FTOASTR (buf, sizeof buf, 0, 0, x);                     \
                xprintf ("%*s", adjusted_width, buf))

PRINT_TYPE (print_s_char, signed char)
PRINT_TYPE (print_char, unsigned char)
PRINT_TYPE (print_s_short, short int)
PRINT_TYPE (print_short, unsigned short int)
PRINT_TYPE (print_int, unsigned int)
PRINT_TYPE (print_long, unsigned long int)
PRINT_TYPE (print_long_long, unsigned_long_long_int)

PRINT_FLOATTYPE (print_float, float, ftoastr, FLT_BUFSIZE_BOUND)
PRINT_FLOATTYPE (print_double, double, dtoastr, DBL_BUFSIZE_BOUND)
PRINT_FLOATTYPE (print_long_double, long double, ldtoastr, LDBL_BUFSIZE_BOUND)


