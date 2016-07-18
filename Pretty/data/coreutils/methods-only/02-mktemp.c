static int
mkstemp_len (char *tmpl, size_t suff_len, size_t x_len, bool dry_run)
{
  return gen_tempname_len (tmpl, suff_len, 0, dry_run ? GT_NOCREATE : GT_FILE,
                           x_len);
}

