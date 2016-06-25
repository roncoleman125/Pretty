/* Return nonzero if the Ith character of escaped string ES matches C
   and is not escaped itself.  */
static inline bool
es_match (struct E_string const *es, size_t i, char c)
{
  return es->s[i] == c && !es->escaped[i];
}

/* When true, each sequence in the input of a repeated character
   (call it c) is replaced (in the output) by a single occurrence of c
   for every c in the squeeze set.  */
static bool squeeze_repeats = false;

/* When true, removes characters in the delete set from input.  */
static bool delete = false;

/* Use the complement of set1 in place of set1.  */
static bool complement = false;

/* When tr is performing translation and string1 is longer than string2,
   POSIX says that the result is unspecified.  That gives the implementor
   of a POSIX conforming version of tr two reasonable choices for the
   semantics of this case.

   * The BSD tr pads string2 to the length of string1 by
   repeating the last character in string2.

   * System V tr ignores characters in string1 that have no
   corresponding character in string2.  That is, string1 is effectively
   truncated to the length of string2.

   When nonzero, this flag causes GNU tr to imitate the behavior
   of System V tr when translating with string1 longer than string2.
   The default is to emulate BSD tr.  This flag is ignored in modes where
   no translation is performed.  Emulating the System V tr
   in this exceptional case causes the relatively common BSD idiom:

       tr -cs A-Za-z0-9 '\012'

   to break (it would convert only zero bytes, rather than all
   non-alphanumerics, to newlines).

   WARNING: This switch does not provide general BSD or System V
   compatibility.  For example, it doesn't disable the interpretation
   of the POSIX constructs [:alpha:], [=c=], and [c*10], so if by
   some unfortunate coincidence you use such constructs in scripts
   expecting to use some other version of tr, the scripts will break.  */
static bool truncate_set1 = false;

/* An alias for (!delete && non_option_args == 2).
   It is set in main and used there and in validate().  */
static bool translating;

static char io_buf[BUFSIZ];

static char const *const char_class_name[] =
{
  "alnum", "alpha", "blank", "cntrl", "digit", "graph",
  "lower", "print", "punct", "space", "upper", "xdigit"
};

/* Array of boolean values.  A character `c' is a member of the
   squeeze set if and only if in_squeeze_set[c] is true.  The squeeze
   set is defined by the last (possibly, the only) string argument
   on the command line when the squeeze option is given.  */
static bool in_squeeze_set[N_CHARS];

/* Array of boolean values.  A character `c' is a member of the
   delete set if and only if in_delete_set[c] is true.  The delete
   set is defined by the first (or only) string argument on the
   command line when the delete option is given.  */
static bool in_delete_set[N_CHARS];

/* Array of character values defining the translation (if any) that
   tr is to perform.  Translation is performed only when there are
   two specification strings and the delete switch is not given.  */
static char xlate[N_CHARS];

static struct option const long_options[] =
{
  {"complement", no_argument, NULL, 'c'},
  {"delete", no_argument, NULL, 'd'},
  {"squeeze-repeats", no_argument, NULL, 's'},
  {"truncate-set1", no_argument, NULL, 't'},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {NULL, 0, NULL, 0}
};

