/* Accumulate directory data.  */
static inline void
duinfo_add (struct duinfo *a, struct duinfo const *b)
{
  a->size += b->size;
  if (timespec_cmp (a->tmax, b->tmax) < 0)
    a->tmax = b->tmax;
}

/* A structure for per-directory level information.  */
struct dulevel
{
  /* Entries in this directory.  */
  struct duinfo ent;

  /* Total for subdirectories.  */
  struct duinfo subdir;
};

/* If true, display counts for all files, not just directories.  */
static bool opt_all = false;

/* If true, rather than using the disk usage of each file,
   use the apparent size (a la stat.st_size).  */
static bool apparent_size = false;

/* If true, count each hard link of files with multiple links.  */
static bool opt_count_all = false;

/* If true, hash all files to look for hard links.  */
static bool hash_all;

/* If true, output the NUL byte instead of a newline at the end of each line. */
static bool opt_nul_terminate_output = false;

/* If true, print a grand total at the end.  */
static bool print_grand_total = false;

/* If nonzero, do not add sizes of subdirectories.  */
static bool opt_separate_dirs = false;

/* Show the total for each directory (and file if --all) that is at
   most MAX_DEPTH levels down from the root of the hierarchy.  The root
   is at level 0, so `du --max-depth=0' is equivalent to `du -s'.  */
static size_t max_depth = SIZE_MAX;

/* Human-readable options for output.  */
static int human_output_opts;

/* If true, print most recently modified date, using the specified format.  */
static bool opt_time = false;

/* Type of time to display. controlled by --time.  */

enum time_type
  {
    time_mtime,			/* default */
    time_ctime,
    time_atime
  };

static enum time_type time_type = time_mtime;

/* User specified date / time style */
static char const *time_style = NULL;

/* Format used to display date / time. Controlled by --time-style */
static char const *time_format = NULL;

/* The units to use when printing sizes.  */
static uintmax_t output_block_size;

/* File name patterns to exclude.  */
static struct exclude *exclude;

/* Grand total size of all args, in bytes. Also latest modified date. */
static struct duinfo tot_dui;

  ((Type) == FTS_DP		\
   || (Type) == FTS_DNR)

/* For long options that have no equivalent short option, use a
   non-character as a pseudo short option, starting with CHAR_MAX + 1.  */
enum
{
  APPARENT_SIZE_OPTION = CHAR_MAX + 1,
  EXCLUDE_OPTION,
  FILES0_FROM_OPTION,
  HUMAN_SI_OPTION,
  FTS_DEBUG,
  TIME_OPTION,
  TIME_STYLE_OPTION
};

static struct option const long_options[] =
{
  {"all", no_argument, NULL, 'a'},
  {"apparent-size", no_argument, NULL, APPARENT_SIZE_OPTION},
  {"block-size", required_argument, NULL, 'B'},
  {"bytes", no_argument, NULL, 'b'},
  {"count-links", no_argument, NULL, 'l'},
  /* {"-debug", no_argument, NULL, FTS_DEBUG}, */
  {"dereference", no_argument, NULL, 'L'},
  {"dereference-args", no_argument, NULL, 'D'},
  {"exclude", required_argument, NULL, EXCLUDE_OPTION},
  {"exclude-from", required_argument, NULL, 'X'},
  {"files0-from", required_argument, NULL, FILES0_FROM_OPTION},
  {"human-readable", no_argument, NULL, 'h'},
  {"si", no_argument, NULL, HUMAN_SI_OPTION},
  {"max-depth", required_argument, NULL, 'd'},
  {"null", no_argument, NULL, '0'},
  {"no-dereference", no_argument, NULL, 'P'},
  {"one-file-system", no_argument, NULL, 'x'},
  {"separate-dirs", no_argument, NULL, 'S'},
  {"summarize", no_argument, NULL, 's'},
  {"total", no_argument, NULL, 'c'},
  {"time", optional_argument, NULL, TIME_OPTION},
  {"time-style", required_argument, NULL, TIME_STYLE_OPTION},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {NULL, 0, NULL, 0}
};

static char const *const time_args[] =
{
  "atime", "access", "use", "ctime", "status", NULL
};
static enum time_type const time_types[] =
{
  time_atime, time_atime, time_atime, time_ctime, time_ctime
};
ARGMATCH_VERIFY (time_args, time_types);

/* `full-iso' uses full ISO-style dates and times.  `long-iso' uses longer
   ISO-style time stamps, though shorter than `full-iso'.  `iso' uses shorter
   ISO-style time stamps.  */
enum time_style
  {
    full_iso_time_style,       /* --time-style=full-iso */
    long_iso_time_style,       /* --time-style=long-iso */
    iso_time_style	       /* --time-style=iso */
  };

static char const *const time_style_args[] =
{
  "full-iso", "long-iso", "iso", NULL
};
static enum time_style const time_style_types[] =
{
  full_iso_time_style, long_iso_time_style, iso_time_style
};
ARGMATCH_VERIFY (time_style_args, time_style_types);

