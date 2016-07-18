/* Encapsulate the test for whether the errno value, ERRNUM, is ignorable.  */
static inline bool
ignorable_missing (struct rm_options const *x, int errnum)
{
  return x->ignore_missing_files && nonexistent_file_errno (errnum);
}

