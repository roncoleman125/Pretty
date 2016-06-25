/* Print size (and optionally time) indicated by *PDUI, followed by STRING.  */

static void
print_size (const struct duinfo *pdui, const char *string)
{
  print_only_size (pdui->size);
  if (opt_time)
    {
      putchar ('\t');
      show_date (time_format, pdui->tmax);
    }
  printf ("\t%s%c", string, opt_nul_terminate_output ? '\0' : '\n');
  fflush (stdout);
}

