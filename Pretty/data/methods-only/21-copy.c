/* Set *X to the default options for a value of type struct cp_options.  */

extern void
cp_options_default (struct cp_options *x)
{
  memset (x, 0, sizeof *x);
  {
    priv_set_t *pset = priv_allocset ();
    if (!pset)
      xalloc_die ();
    if (getppriv (PRIV_EFFECTIVE, pset) == 0)
      {
        x->chown_privileges = priv_ismember (pset, PRIV_FILE_CHOWN);
        x->owner_privileges = priv_ismember (pset, PRIV_FILE_OWNER);
      }
    priv_freeset (pset);
  }
  x->chown_privileges = x->owner_privileges = (geteuid () == 0);
}

