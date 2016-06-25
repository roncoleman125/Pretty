static bool
valid_file_spec (struct File_spec const *f)
{
  /* Exactly one of the following subexpressions must be true. */
  return ((f->fd == -1) ^ (f->errnum == 0));
}

