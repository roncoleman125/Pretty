static int
mkdtemp_len (char *tmpl, size_t suff_len, size_t x_len, bool dry_run)
{
  return gen_tempname_len (tmpl, suff_len, 0, dry_run ? GT_NOCREATE : GT_DIR,
                           x_len);
}

/* True if we have already closed standard output.  */
static bool stdout_closed;

