/* Output a color indicator (which may contain nulls).  */
static void
put_indicator (const struct bin_str *ind)
{
  if (! used_color)
    {
      used_color = true;
      prep_non_filename_text ();
    }

  fwrite (ind->string, ind->len, 1, stdout);
}

