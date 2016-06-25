/* TEMP represents a new process; add it to the process table.  Create
   the process table the first time it's called.  */

static void
register_proc (struct tempnode *temp)
{
  if (! proctab)
    {
      proctab = hash_initialize (INIT_PROCTAB_SIZE, NULL,
                                 proctab_hasher,
                                 proctab_comparator,
                                 NULL);
      if (! proctab)
        xalloc_die ();
    }

  temp->state = UNREAPED;

  if (! hash_insert (proctab, temp))
    xalloc_die ();
}

