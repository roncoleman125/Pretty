/* If FORMAT is a valid printf format for a double argument, return
   its long double equivalent, allocated from dynamic storage, and
   store into *LAYOUT a description of the output layout; otherwise,
   report an error and exit.  */

static char const *
long_double_format (char const *fmt, struct layout *layout)
{
  size_t i;
  size_t prefix_len = 0;
  size_t suffix_len = 0;
  size_t length_modifier_offset;
  bool has_L;

  for (i = 0; ! (fmt[i] == '%' && fmt[i + 1] != '%'); i += (fmt[i] == '%') + 1)
    {
      if (!fmt[i])
        error (EXIT_FAILURE, 0,
               _("format %s has no %% directive"), quote (fmt));
      prefix_len++;
    }

  i++;
  i += strspn (fmt + i, "-+#0 '");
  i += strspn (fmt + i, "0123456789");
  if (fmt[i] == '.')
    {
      i++;
      i += strspn (fmt + i, "0123456789");
    }

  length_modifier_offset = i;
  has_L = (fmt[i] == 'L');
  i += has_L;
  if (fmt[i] == '\0')
    error (EXIT_FAILURE, 0, _("format %s ends in %%"), quote (fmt));
  if (! strchr ("efgaEFGA", fmt[i]))
    error (EXIT_FAILURE, 0,
           _("format %s has unknown %%%c directive"), quote (fmt), fmt[i]);

  for (i++; ; i += (fmt[i] == '%') + 1)
    if (fmt[i] == '%' && fmt[i + 1] != '%')
      error (EXIT_FAILURE, 0, _("format %s has too many %% directives"),
             quote (fmt));
    else if (fmt[i])
      suffix_len++;
    else
      {
        size_t format_size = i + 1;
        char *ldfmt = xmalloc (format_size + 1);
        memcpy (ldfmt, fmt, length_modifier_offset);
        ldfmt[length_modifier_offset] = 'L';
        strcpy (ldfmt + length_modifier_offset + 1,
                fmt + length_modifier_offset + has_L);
        layout->prefix_len = prefix_len;
        layout->suffix_len = suffix_len;
        return ldfmt;
      }
}

