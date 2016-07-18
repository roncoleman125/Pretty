/* Check compatibility of ordering options.  */

static void
check_ordering_compatibility (void)
{
  struct keyfield *key;

  for (key = keylist; key; key = key->next)
    if (1 < (key->numeric + key->general_numeric + key->human_numeric
             + key->month + (key->version | key->random | !!key->ignore)))
      {
        /* The following is too big, but guaranteed to be "big enough".  */
        char opts[sizeof short_options];
        /* Clear flags we're not interested in.  */
        key->skipsblanks = key->skipeblanks = key->reverse = false;
        key_to_opts (key, opts);
        incompatible_options (opts);
      }
}

