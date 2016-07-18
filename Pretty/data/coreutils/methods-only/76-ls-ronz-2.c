static size_t
length_of_file_name_and_frills (const struct fileinfo *f)
{
  size_t len = 0;
  size_t name_width;
  char buf[MAX (LONGEST_HUMAN_READABLE + 1, INT_BUFSIZE_BOUND (uintmax_t))];

  if (print_inode)
    len += 1 + (format == with_commas
                ? strlen (umaxtostr (f->stat.st_ino, buf))
                : inode_number_width);

  if (print_block_size)
    len += 1 + (format == with_commas
                ? strlen (! f->stat_ok ? "?"
                          : human_readable (ST_NBLOCKS (f->stat), buf,
                                            human_output_opts, ST_NBLOCKSIZE,
                                            output_block_size))
                : block_size_width);

  if (print_scontext)
    len += 1 + (format == with_commas ? strlen (f->scontext) : scontext_width);

  quote_name (NULL, f->name, filename_quoting_options, &name_width);
  len += name_width;

  if (indicator_style != none)
    {
      char c = get_type_indicator (f->stat_ok, f->stat.st_mode, f->filetype);
      len += (c != 0);
    }

  return len;
}

