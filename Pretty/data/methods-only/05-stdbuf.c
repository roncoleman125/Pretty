/* Populate environ with _STDBUF_I=$MODE _STDBUF_O=$MODE _STDBUF_E=$MODE  */

static void
set_libstdbuf_options (void)
{
  int i;

  for (i = 0; i < ARRAY_CARDINALITY (stdbuf); i++)
    {
      if (stdbuf[i].optarg)
        {
          char *var;
          int ret;

          if (*stdbuf[i].optarg == 'L')
            ret = asprintf (&var, "%s%c=L", "_STDBUF_",
                            toupper (stdbuf[i].optc));
          else
            ret = asprintf (&var, "%s%c=%" PRIuMAX, "_STDBUF_",
                            toupper (stdbuf[i].optc),
                            (uintmax_t) stdbuf[i].size);
          if (ret < 0)
            xalloc_die ();

          if (putenv (var) != 0)
            {
              error (EXIT_CANCELED, errno,
                     _("failed to update the environment with %s"),
                     quote (var));
            }
        }
    }
}

