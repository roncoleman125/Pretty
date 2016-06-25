/* Compare file versions.
   Unlike all other compare functions above, cmp_version depends only
   on filevercmp, which does not fail (even for locale reasons), and does not
   need a secondary sort key. See lib/filevercmp.h for function description.

   All the other sort options, in fact, need xstrcoll and strcmp variants,
   because they all use a string comparison (either as the primary or secondary
   sort key), and xstrcoll has the ability to do a longjmp if strcoll fails for
   locale reasons.  Lastly, filevercmp is ALWAYS available with gnulib.  */
static inline int
cmp_version (struct fileinfo const *a, struct fileinfo const *b)
{
  return filevercmp (a->name, b->name);
}

